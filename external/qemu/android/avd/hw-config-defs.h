
#ifndef HWCFG_INT
#error  HWCFG_INT not defined
#endif
#ifndef HWCFG_BOOL
#error  HWCFG_BOOL not defined
#endif
#ifndef HWCFG_DISKSIZE
#error  HWCFG_DISKSIZE not defined
#endif
#ifndef HWCFG_STRING
#error  HWCFG_STRING not defined
#endif
#ifndef HWCFG_DOUBLE
#error  HWCFG_DOUBLE not defined
#endif

HWCFG_INT(
  hw_ramSize,
  "hw.ramSize",
  96,
  "Device ram size",
  "The amount of physical RAM on the device, in megabytes.")

HWCFG_BOOL(
  hw_touchScreen,
  "hw.touchScreen",
  "yes",
  "Touch-screen support",
  "Whether there is a touch screen or not on the device.")

HWCFG_BOOL(
  hw_trackBall,
  "hw.trackBall",
  "yes",
  "Track-ball support",
  "Whether there is a trackball on the device.")

HWCFG_BOOL(
  hw_keyboard,
  "hw.keyboard",
  "yes",
  "Keyboard support",
  "Whether the device has a QWERTY keyboard.")

HWCFG_BOOL(
  hw_dPad,
  "hw.dPad",
  "yes",
  "DPad support",
  "Whether the device has DPad keys")

HWCFG_BOOL(
  hw_gsmModem,
  "hw.gsmModem",
  "yes",
  "GSM modem support",
  "Whether there is a GSM modem in the device.")

HWCFG_BOOL(
  hw_camera,
  "hw.camera",
  "no",
  "Camera support",
  "Whether the device has a camera.")

HWCFG_INT(
  hw_camera_maxHorizontalPixels,
  "hw.camera.maxHorizontalPixels",
  640,
  "Maximum horizontal camera pixels",
  "")

HWCFG_INT(
  hw_camera_maxVerticalPixels,
  "hw.camera.maxVerticalPixels",
  480,
  "Maximum vertical camera pixels",
  "")

HWCFG_BOOL(
  hw_gps,
  "hw.gps",
  "yes",
  "GPS support",
  "Whether there is a GPS in the device.")

HWCFG_BOOL(
  hw_battery,
  "hw.battery",
  "yes",
  "Battery support",
  "Whether the device can run on a battery.")

HWCFG_BOOL(
  hw_accelerometer,
  "hw.accelerometer",
  "yes",
  "Accelerometer",
  "Whether there is an accelerometer in the device.")

HWCFG_BOOL(
  hw_magneticField,
  "hw.magneticField",
  "yes",
  "Magnetic field sensor support",
  "Whether there is a magnetic field sensor in the device.")

HWCFG_BOOL(
  hw_orientation,
  "hw.orientation",
  "yes",
  "Orientation sensor support",
  "Whether there is an orientation sensor in the device.")

HWCFG_BOOL(
  hw_temperature,
  "hw.temperature",
  "yes",
  "Temperature sensor support",
  "Whether there is a temperature sensor in the device.")

HWCFG_BOOL(
  hw_audioInput,
  "hw.audioInput",
  "yes",
  "Audio recording support",
  "Whether the device can record audio")

HWCFG_BOOL(
  hw_audioOutput,
  "hw.audioOutput",
  "yes",
  "Audio playback support",
  "Whether the device can play audio")

HWCFG_BOOL(
  hw_sdCard,
  "hw.sdCard",
  "yes",
  "SD Card support",
  "Whether the device supports insertion/removal of virtual SD Cards.")

HWCFG_BOOL(
  disk_cachePartition,
  "disk.cachePartition",
  "yes",
  "Cache partition support",
  "Whether we use a /cache partition on the device.")

HWCFG_DISKSIZE(
  disk_cachePartition_size,
  "disk.cachePartition.size",
  "66MB",
  "Cache partition size",
  "")

HWCFG_INT(
  hw_lcd_density,
  "hw.lcd.density",
  160,
  "Abstracted LCD density",
  "Must be one of 120, 160 or 240. A value used to roughly describe the density of the LCD screen for automatic resource/asset selection.")

HWCFG_INT(
  vm_heapSize,
  "vm.heapSize",
  16,
  "Max VM application heap size",
  "The maximum heap size a Dalvik application might allocate before being killed by the system. Value is in megabytes.")

#undef HWCFG_INT
#undef HWCFG_BOOL
#undef HWCFG_DISKSIZE
#undef HWCFG_STRING
#undef HWCFG_DOUBLE
/* end of auto-generated file */
