#!/bin/bash
set -eu
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/raw build/clean
cd build
test -x catcam || { echo 'Run python3 tools/build.py first.' >&2; exit 1; }
python3 "$root/audio/yarn_sofa.py" yarn-and-sofa.wav
python3 "$root/audio/mix.py" yarn-and-sofa.wav soundtrack.wav
if [[ "${1:-}" == "--preview" ]]; then
  ./catcam --film --fps 24 --from 0 --to 479 --w 640 --h 360 --spp 8 --exp 1.14 --seg preview-video.mp4
  ffmpeg -v error -y -i preview-video.mp4 -i soundtrack.wav -map 0:v:0 -map 1:a:0 -c:v copy -c:a aac -b:a 192k -movflags +faststart preview.mp4
  exit 0
fi
oidn="${OIDN_LIB_DIR:-}"
if [[ -z "$oidn" ]]; then oidn="$(brew --prefix open-image-denoise)/lib"; fi
clang++ -std=c++17 -O2 "$root/tools/denoise.cpp" -L"$oidn" -Wl,-rpath,"$oidn" -lOpenImageDenoise -o denoise
for ((f=0; f<480; ++f)); do
  path=$(printf 'raw/f_%05d.ppm' "$f")
  if [[ ! -f "$path" || $(stat -f %z "$path") != 4320016 ]]; then
    ./catcam --film --fps 24 --from "$f" --to 479 --w 1600 --h 900 --spp 320 --pass 8 --bands 2 --exp 1.14 --frames raw
    break
  fi
done
for ((f=0; f<480; ++f)); do echo "$f"; done | ./denoise raw clean
ffmpeg -v error -y -framerate 24 -i clean/f_%05d.ppm -i soundtrack.wav -map 0:v -map 1:a \
  -vf 'hqdn3d=0.5:0.45:1.8:1.3,scale=out_color_matrix=bt709:out_range=tv,format=yuv420p' \
  -colorspace bt709 -color_primaries bt709 -color_trc bt709 -color_range tv \
  -c:v libx264 -preset slow -crf 16 -c:a aac -b:a 192k -movflags +faststart rental-cat-pov.mp4
