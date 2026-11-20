"""Build the scene on Apple Silicon using the pinned external Cat Garden renderer."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
integration = root / 'integration'
lock = json.loads((integration / 'upstream.json').read_text())
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--upstream', type=Path, help='Existing upstream cat-garden/src directory')
args = parser.parse_args()
build = root / 'build'
src = build / 'src'
src.mkdir(parents=True, exist_ok=True)
url = 'https://raw.githubusercontent.com/BubbleBrainBB/awesome-fable-5.2/'
for name, checksum in lock['files'].items():
    if args.upstream:
        data = (args.upstream / name).read_bytes()
    else:
        cache = build / 'upstream' / lock['commit'] / name
        data = cache.read_bytes() if cache.is_file() else b''
        if hashlib.sha256(data).hexdigest() != checksum:
            data = subprocess.check_output([
                'curl', '--fail', '--silent', '--show-error', '--location',
                '--retry', '3', '--connect-timeout', '10', '--max-time', '60',
                f"{url}{lock['commit']}/cat-garden/src/{name}"
            ])
            if hashlib.sha256(data).hexdigest() == checksum:
                cache.parent.mkdir(parents=True, exist_ok=True)
                cache.write_bytes(data)
    if hashlib.sha256(data).hexdigest() != checksum:
        raise ValueError(f'Unexpected upstream revision for {name}')
    edits = integration / (name + '.json')
    if edits.is_file():
        source = data.decode()
        for edit in reversed(json.loads(edits.read_text())):
            start = edit['offset']
            insert = ((integration / edit['file']).read_text()
                      if 'file' in edit else edit['insert'])
            source = source[:start] + insert + source[start + edit['remove']:]
        data = source.encode()
    (src / name).write_bytes(data)

for file in (root / 'scene').iterdir():
    shutil.copyfile(file, src / file.name)
subprocess.run(['clang++', '-std=c++17', '-O2', '-fobjc-arc', '-Wno-deprecated-declarations',
                'src/main.mm', '-framework', 'Metal', '-framework', 'Foundation',
                '-framework', 'CoreGraphics', '-o', 'catcam'], cwd=build, check=True)
