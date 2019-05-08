{
  "targets": [
    {
      "target_name": "bcomm",
      "sources": [ "src/bcomm.cpp" ],
      "ldflags": ["-lbluetooth"]
    },
    {
      "target_name": "wrapper",
      "sources": [
        "cpp-api/lib/bitalino.cpp",
        "cpp-api/bitalino-adapter.cpp",
        "cpp-api/bitalino-api.cpp"
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '-DHASBLUETOOTH',
          ]
        }]
      ],
      "ldflags": ["-lbluetooth"]
    }
  ]
}
