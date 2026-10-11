# Distribution notice

This repository contains the XTAlt PixInsight integration source code, build
automation, documentation, and tests.

The following items are intentionally excluded:

- ONNX model files and decrypted computational graphs
- application runtimes and extracted third-party program files
- PixInsight `.xssk` private signing keys
- signatures created with a Local Signing Identity
- generated binaries and build caches

Users must obtain all required models, runtimes, and third-party software through
channels that grant them the necessary rights. Product names and trademarks belong
to their respective owners.

A public binary release of this module requires a valid Certified PixInsight
Developer code signature. Local developer signatures are intended for development
and testing on the signing installation.
