#!/bin/bash

LIB=bazel-bin/lyra/android_example/libandroid_lyra.so
LIB_PATCH=./libandroid_lyra.so.patch
FUNC_NAME=_GLOBAL__sub_I_stacktrace.cc
NDK_OBJ=~/android/sdk/ndk/21.4.7075529/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/aarch64-linux-android/bin/objdump


CMD="nm -n \"$LIB\" | c++filt | grep \"$FUNC_NAME\" | awk '{print \$1}'"
ADDR_HEX=$(eval "$CMD" | tr -d '\n' | xargs)
ADDR_DEC=$((16#$ADDR_HEX))

LINE_HEX=$($NDK_OBJ -d -C "$LIB" \
  --start-address=0x$ADDR_HEX --stop-address=$((ADDR_DEC+0x30)) \
  | grep _Unwind_Backtrace | head -1 | awk '{print $1}' | sed 's/://')


OFFSET=$((16#$LINE_HEX))

echo "📍 Found _Unwind_Backtrace call at offset 0x$LINE_HEX"
echo "🛠  Will patch $LIB at offset 0x$OFFSET with nop (d503201f)"

cp "$LIB" "$LIB_PATCH"
printf '\x1f\x20\x03\xd5' | dd of="$LIB_PATCH" bs=1 seek=$OFFSET count=4 conv=notrunc status=none
touch "$LIB_PATCH"
echo "✅ Patch complete. create patched lib as $LIB_PATCH"
echo "Check the patch..."
value=$(hexdump -s $OFFSET -n 4 -e '1/4 "%08x"' "$LIB_PATCH") #  (== d503201f, NOP)
if [[ "$value" == "d503201f" ]]; then
  echo "ok"
else
  echo "fail"
fi

echo "🔍 _Unwind_Backtrace(before)"
$NDK_OBJ -d -C "$LIB" \
  --start-address=0x$ADDR_HEX \
  --stop-address=$((ADDR_DEC + 0x30))

echo "(after)"
$NDK_OBJ -d -C "$LIB_PATCH" \
  --start-address=0x$ADDR_HEX \
  --stop-address=$((ADDR_DEC + 0x30))