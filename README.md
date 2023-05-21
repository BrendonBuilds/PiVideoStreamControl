# PiVideoStreamControl
Remotely control libcamera-vid from Windows/Ubuntu + gstreamer


Requirements:
- Qt 5.15 installed on Windows/Ubuntu

- SSH is installed, added to PATH/available from CLI

- Pre-shared keys to prevent password prompts when using SSH with Raspberry Pi: https://danidudas.medium.com/how-to-connect-to-raspberry-pi-via-ssh-without-password-using-ssh-keys-3abd782688a

- Rebuild QtMultimedia with GStreamer Support (Windows only): https://github.com/gunrot/windows-qt-with-gstreamer


For this software to run:

- You must be able to connect to your raspberry pi from the command line via `ssh username@ip.add.re.ss` without having to enter a password.

- Remote stream on the raspberry pi could be started with a command similar to `libcamera-vid --camera 0 -t 0 --width 1456 --height 1088 --awb indoor --inline --listen -o tcp://0.0.0.0:8888 --framerate 50 --shutter 2800 --gain 1`

- You must be able to view the remote stream from the command line via `gst-launch-1.0 tcpclientsrc host=ip.add.re.ss port=8888 ! decodebin ! videoconvert ! autovideosink`
