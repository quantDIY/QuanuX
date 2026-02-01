# Centralized SDKs Repository

This directory contains proprietary or external SDKs injected by the user.

## Structure
-   `twsapi/`: Interactive Brokers TWS API C++ SDK.
-   `onixs/`: OnixS DirectConnect SDK.

## Usage
Extensions link against these SDKs by referencing `extensions/sdks/<sdk_name>`.
Use `quanuxctl integrate <extension_name>` to populate these folders from your local machine.
