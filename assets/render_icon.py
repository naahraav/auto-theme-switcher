from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "src" / "app.ico"
SIZES = (16, 20, 24, 32, 40, 48, 64, 128, 256)
SCALE = 8
LIGHT = (251, 191, 36, 255)
SUN = (255, 251, 235, 255)
DARK = (15, 23, 42, 255)
MOON = (248, 250, 252, 255)
RING = (14, 165, 233, 255)


def box(x0, y0, x1, y1):
    return (x0 * SCALE, y0 * SCALE, x1 * SCALE, y1 * SCALE)


def crescent_mask(simple):
    radius = 12 if simple else 11
    offset = (radius * radius - 81) ** 0.5
    mask = Image.new("L", (64 * SCALE, 64 * SCALE), 0)
    draw = ImageDraw.Draw(mask)
    draw.ellipse(
        box(46 - offset - radius, 43 - radius, 46 - offset + radius, 43 + radius),
        fill=255,
    )
    draw.ellipse(box(37, 34, 55, 52), fill=0)
    return mask.rotate(25, resample=Image.Resampling.BICUBIC, center=(42 * SCALE, 43 * SCALE))


def render(simple=False):
    canvas = Image.new("RGBA", (64 * SCALE, 64 * SCALE), (0, 0, 0, 0))
    clip = Image.new("L", canvas.size, 0)
    ImageDraw.Draw(clip).ellipse(box(6, 6, 58, 58), fill=255)

    base = Image.new("RGBA", canvas.size, (0, 0, 0, 0))
    base_draw = ImageDraw.Draw(base)
    base_draw.polygon([(0, 0), (64 * SCALE, 0), (0, 64 * SCALE)], fill=LIGHT)
    if not simple:
        base_draw.ellipse(box(14, 14, 26, 26), fill=SUN)
    base_draw.polygon(
        [(64 * SCALE, 0), (64 * SCALE, 64 * SCALE), (0, 64 * SCALE)], fill=DARK
    )
    base.putalpha(clip)

    canvas.alpha_composite(base)

    moon = Image.new("RGBA", canvas.size, MOON)
    moon.putalpha(crescent_mask(simple))
    canvas.alpha_composite(moon)

    ImageDraw.Draw(canvas).ellipse(
        box(6, 6, 58, 58), outline=RING, width=(6 if simple else 5) * SCALE
    )
    return canvas


def main():
    full = render()
    compact = render(simple=True)
    frames = [
        (compact if size <= 24 else full).resize((size, size), Image.Resampling.LANCZOS)
        for size in SIZES
    ]
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    frames[-1].save(
        OUTPUT,
        format="ICO",
        sizes=[(size, size) for size in SIZES],
        append_images=frames[:-1],
        bitmap_format="bmp",
    )
    print(f"Wrote {OUTPUT.relative_to(ROOT)} ({', '.join(str(s) for s in SIZES)} px)")


if __name__ == "__main__":
    main()
