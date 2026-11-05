"""Render captured program transcripts as readable PNG evidence."""

from pathlib import Path
import textwrap

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / "build" / "evidence"
OUTPUT = ROOT / "docs" / "images"
FONT_PATH = Path(r"C:\Windows\Fonts\consola.ttf")


def render(title, source_names, output_name):
    font = ImageFont.truetype(str(FONT_PATH), 22)
    title_font = ImageFont.truetype(str(FONT_PATH), 29)
    lines = []
    for source_name in source_names:
        lines.append(f"$ {source_name}")
        text = (EVIDENCE / source_name).read_text(encoding="utf-8-sig")
        for line in text.splitlines():
            lines.extend(textwrap.wrap(line, width=112,
                                       subsequent_indent="  ") or [""])
        lines.append("")

    line_height = 29
    width = 1720
    height = 105 + line_height * len(lines)
    image = Image.new("RGB", (width, height), "#f7f8fa")
    draw = ImageDraw.Draw(image)
    draw.rectangle((0, 0, width, 72), fill="#17324d")
    draw.text((28, 19), title, font=title_font, fill="white")
    y = 88
    for line in lines:
        color = "#176b54" if line.startswith("$") else "#17212b"
        draw.text((28, y), line, font=font, fill=color)
        y += line_height
    OUTPUT.mkdir(parents=True, exist_ok=True)
    image.save(OUTPUT / output_name)


render("Observed topology-analysis output",
       ["line-analysis.txt", "shortcut-analysis.txt",
        "disconnected-analysis.txt"],
       "topology-results.png")
render("Observed XOR training output", ["xor-training.txt"],
       "xor-training.png")
