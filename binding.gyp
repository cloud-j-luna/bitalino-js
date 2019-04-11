{
  "targets": [
    {
      "target_name": "bcomm",
      "sources": [ "src/bcomm.cpp" ],
      "ldflags": ["-lbluetooth"]
    },

    {
      "target_name": "cpp-bitalino",
      "sources": [
        "cpp-api/lib/bitalino.cpp"
        "cpp-api/bitalino-adapter.cpp",
        "cpp-api/bitalino-api.cpp"
      ],
      "ldflags": ["-lbluetooth"],
      "cflags": ["-DHASBLUETOOTH"]
    }
  ]
}
