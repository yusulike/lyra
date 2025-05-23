load("@rules_android//android:rules.bzl", "android_binary", "android_library")

# Placeholder for jni import

package(default_visibility = ["//visibility:public"])

licenses(["notice"])


cc_library(
    name = "android_lyra_lib",
    srcs = [
        "android_lyra_lib.cc"
    ],
    hdrs = ["android_lyra_lib.h"],
    # linkopts = [
    #     "-landroid",
    #     "-ldl",
    #     "-llog",
    # ],
    deps = [
        # "//lyra:lyra_benchmark_lib",
        "//lyra:lyra_config",
        "//lyra/cli_example:decoder_main_lib",
        "//lyra/cli_example:encoder_main_lib",
        "@com_google_absl//absl/random",
    ],
    linkopts = ["-shared"], 
    copts = ["-fPIC"],
    alwayslink = 1
)

cc_binary(
    name = "libandroid_lyra.so",
    deps = [":android_lyra_lib"],
    linkshared = 1,
    linkstatic = 1,
    linkopts = ["-Wl,--no-as-needed"], 
)

# Android App rules
cc_library(
    name = "jni_lyra_benchmark_lib",
    srcs = [
        "jni_lyra_benchmark_lib.cc",
        "@org_tensorflow//tensorflow/lite:libtensorflowlite.so",
    ],
    linkopts = [
        "-landroid",
        "-ldl",
        "-llog",
    ],
    deps = [
        "//lyra:lyra_benchmark_lib",
        "//lyra:lyra_config",
        "//lyra/cli_example:decoder_main_lib",
        "//lyra/cli_example:encoder_main_lib",
        "@com_google_absl//absl/random",
    ],
    alwayslink = True,
)

android_library(
    name = "lyra_android_lib",
    srcs = ["//lyra/android_example/java/com/example/android/lyra:MainActivity.java"],
    custom_package = "com.example.android.lyra",
    manifest = "LibraryManifest.xml",
    resource_files = glob(["res/**/*"]),
    deps = [
        ":jni_lyra_benchmark_lib",
        "@maven//:androidx_annotation_annotation",
        "@maven//:androidx_appcompat_appcompat",
        "@maven//:androidx_constraintlayout_constraintlayout",
        "@maven//:androidx_core_core",
        "@org_tensorflow//tensorflow/lite:libtensorflowlite.so",
    ],
)

filegroup(
    name = "assets",
    srcs = ["//lyra:android_example_assets"],
)

android_binary(
    name = "lyra_android_example",
    assets = [
        ":assets",
    ],
    assets_dir = "model_coeffs",
    custom_package = "com.example.android.lyra",
    manifest = "AndroidManifest.xml",
    multidex = "native",
    deps = [":lyra_android_lib"],
)
