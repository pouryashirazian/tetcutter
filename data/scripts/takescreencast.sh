echo "INFO: start screen cast using ffmpeg"
#ffmpeg -f x11grab -r 25 -s 1280x800 -i :0.0 -qscale 0 ~/Desktop/out.mp4
ffmpeg -f x11grab -r 25 -s 1680x1050 -i :0.0 -qscale 0 ~/Desktop/out.mp4
