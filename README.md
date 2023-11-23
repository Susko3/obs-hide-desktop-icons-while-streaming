# OBS Plugin: Hide Desktop Icons While Streaming

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  *to protect your privacy when using Display Capture*

**Supported platform: Windows**

<br>

Just a simple plugin that will automaticall hide icons on your desktop while streaming or recording is active.
It does the same as toggling *Show desktop icons* in the right-click menu on the desktop.

When you start streaming or recording, the desktop icons will hide for the duration of your stream or recording.
When `Stop Streaming/Recording` is clicked, the icons will show again.

If, for any reason, the desktop icons don't get restored again (due to a bug in this plugin or OBS crashing), you can always manually show the desktop icons by right-clicking on your desktop.

## How it works

Click on `Start Streaming/Recording` button in OBS → desktop icons are automatically hidden

Click on `Stop Streaming/Recording` button in OBS → desktop icons are automatically shown

If you configured desktop icons to hide when recording _or_ streaming, they will show only after both streaming and recording is stopped.

For example: if you're currently streaming and wish to record a highlight, you don't have to worry about your desktop icons showing when the recording is stopped, as the icons will be neatly hidden until you stop the long-running stream as well.

## Help, I don't see my icons!!

Don't worry, it's very simple to fix 🙂

1. Right-click on your desktop
2. Select `View`
3. Enable `Show desktop icons` as shown in the image below:

![Windows desktop context menu](/image/desktop-context-menu.png)

## Configuration

This plugin can be configured by clicking on `Tools` in the OBS toolbar and then selecting `AutoHide Desktop Icons`.

By clicking on the checkboxes, you can configure desktop icons to hide while streaming or recording. The settings apply when saved, including if you're currently streaming or recording.

![Configuration window](/image/configuration.png)

※ Please note that hiding only when a display capture source is visible is not yet available. The feature is currently in the planning state and might appear in a future version.