#!/bin/bash

# 사용자 정의 설정
LIB=bazel-bin/lyra/android_example/libandroid_lyra.so
LIB_PATCH=./libandroid_lyra.so.patch
FUNC_NAME=_GLOBAL__sub_I_stacktrace.cc
NDK_OBJ=~/android/sdk/ndk/21.4.7075529/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/aarch64-linux-android/bin/objdump

# 1. 함수 주소 추출
ADDR_HEX=$(nm -n "$LIB" | c++filt | grep " $FUNC_NAME" | awk '{print $1}')
ADDR_DEC=$((16#$ADDR_HEX))

# 2. 해당 함수에서 _Unwind_Backtrace를 호출하는 명령어 추출
LINE_HEX=$($NDK_OBJ -d -C "$LIB" \
  --start-address=0x$ADDR_HEX --stop-address=$((ADDR_DEC+0x30)) \
  | grep -B1 "<_Unwind_Backtrace>" | head -1 | awk '{print $1}' | sed 's/://')


# 3. 오프셋 계산
OFFSET=$((16#$LINE_HEX + 4))

# 4. 출력 확인
echo "📍 Found _Unwind_Backtrace call at offset 0x$LINE_HEX"
echo "🛠  Will patch $LIB at offset 0x$OFFSET with nop (d503201f)"

# 5. 패치 수행 (백업하고 4바이트 덮어쓰기)
cp "$LIB" "$LIB_PATCH"
printf '\x1f\x20\x03\xd5' | dd of="$LIB_PATCH" bs=1 seek=$OFFSET count=4 conv=notrunc status=none
touch "$LIB_PATCH"
echo "✅ Patch complete. create patched lib as $LIB_PATCH"
echo "Check the patch (d503201f, NOP):"
hexdump -s $OFFSET -n 4 -e '4/1 "%02x "' "$LIB_PATCH" #  (== d503201f, NOP)
echo ""