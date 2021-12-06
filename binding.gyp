{
  "targets": [
    {
      "target_name": "Zstandard",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-std=c++14", "-fno-rtti", "-fno-exceptions" ],
      "cflags_cc": [ "-std=c++17", "-Wno-implicit-fallthrough" ],
      "sources": [ "Zstandard.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/home/user/Projects/zstd/lib"
      ],
      "libraries": [
          "-lzstd",
          "-L/home/user/Projects/zstd/lib"
      ]
    }
  ]
}
