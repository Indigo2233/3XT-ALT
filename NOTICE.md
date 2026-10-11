# Distribution notice

This repository contains the XTAlt PixInsight integration source code, build
automation, documentation, and tests.

The following items are intentionally excluded from Git history:

- ONNX model files and decrypted computational graphs
- application runtimes and extracted third-party program files
- PixInsight `.xssk` private signing keys
- signatures created with a Local Signing Identity
- generated binaries and build caches

Self-contained GitHub Release assets may include the inference runtime and model files
only when the publisher has confirmed the necessary redistribution rights. The release
packager requires this confirmation explicitly. Product names and trademarks belong to
their respective owners.

A public binary release of this module requires a valid Certified PixInsight
Developer code signature. Local developer signatures are intended for development
and testing on the signing installation.
