"""Build the scene on Apple Silicon using the pinned external Cat Garden renderer."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
from urllib.request import urlopen

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
        with urlopen(f"{url}{lock['commit']}/cat-garden/src/{name}", timeout=60) as response:
            data = response.read()
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
