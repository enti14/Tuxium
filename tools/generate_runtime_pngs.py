#!/usr/bin/env python3
"""
Generate runtime PNG assets during the build:
  - app icon: penguin holding a coin (emoji-inspired)
  - sword icon: PvP sword symbol
"""

from __future__ import annotations

import argparse
import os
import struct
import zlib
from typing import List, Tuple

RGBA = Tuple[int, int, int, int]


def _clamp(v: int) -> int:
    return max(0, min(255, v))


def write_png(path: str, width: int, height: int, pixels: List[List[RGBA]]) -> None:
    signature = b"\x89PNG\r\n\x1a\n"

    def chunk(tag: bytes, data: bytes) -> bytes:
        return (
            struct.pack("!I", len(data))
            + tag
            + data
            + struct.pack("!I", zlib.crc32(tag + data) & 0xFFFFFFFF)
        )

    ihdr = struct.pack("!IIBBBBB", width, height, 8, 6, 0, 0, 0)

    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter type: None
        for x in range(width):
            r, g, b, a = pixels[y][x]
            raw.extend((_clamp(r), _clamp(g), _clamp(b), _clamp(a)))

    idat = zlib.compress(bytes(raw), level=9)
    png = signature + chunk(b"IHDR", ihdr) + chunk(b"IDAT", idat) + chunk(b"IEND", b"")

    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "wb") as f:
        f.write(png)


def canvas(width: int, height: int, color: RGBA = (0, 0, 0, 0)) -> List[List[RGBA]]:
    return [[color for _ in range(width)] for _ in range(height)]


def set_px(img: List[List[RGBA]], x: int, y: int, color: RGBA) -> None:
    h = len(img)
    w = len(img[0]) if h else 0
    if 0 <= x < w and 0 <= y < h:
        img[y][x] = color


def fill_circle(img: List[List[RGBA]], cx: int, cy: int, r: int, color: RGBA) -> None:
    r2 = r * r
    for y in range(cy - r, cy + r + 1):
        for x in range(cx - r, cx + r + 1):
            dx = x - cx
            dy = y - cy
            if dx * dx + dy * dy <= r2:
                set_px(img, x, y, color)


def fill_rect(img: List[List[RGBA]], x0: int, y0: int, x1: int, y1: int, color: RGBA) -> None:
    if x0 > x1:
        x0, x1 = x1, x0
    if y0 > y1:
        y0, y1 = y1, y0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            set_px(img, x, y, color)


def fill_diamond(img: List[List[RGBA]], cx: int, cy: int, rx: int, ry: int, color: RGBA) -> None:
    for y in range(cy - ry, cy + ry + 1):
        for x in range(cx - rx, cx + rx + 1):
            nx = abs(x - cx) / max(rx, 1)
            ny = abs(y - cy) / max(ry, 1)
            if nx + ny <= 1.0:
                set_px(img, x, y, color)


def generate_app_icon(path: str, size: int = 256) -> None:
    img = canvas(size, size)

    # Background gradient
    for y in range(size):
        t = y / max(size - 1, 1)
        c = int(45 + t * 40)
        for x in range(size):
            img[y][x] = (c, c + 18, c + 36, 255)

    # Penguin (emoji-inspired: 🐧)
    fill_circle(img, size // 2, size // 2 + 18, size // 3, (22, 30, 42, 255))  # body
    fill_circle(img, size // 2, size // 2 + 28, size // 5, (246, 246, 246, 255))  # belly
    fill_circle(img, size // 2, size // 2 - 34, size // 5, (22, 30, 42, 255))  # head
    fill_diamond(img, size // 2, size // 2 - 26, 20, 12, (250, 173, 52, 255))  # beak

    # Eyes
    fill_circle(img, size // 2 - 24, size // 2 - 44, 11, (255, 255, 255, 255))
    fill_circle(img, size // 2 + 24, size // 2 - 44, 11, (255, 255, 255, 255))
    fill_circle(img, size // 2 - 24, size // 2 - 44, 4, (0, 0, 0, 255))
    fill_circle(img, size // 2 + 24, size // 2 - 44, 4, (0, 0, 0, 255))

    # Coin in right flipper (emoji-inspired: 🪙)
    coin_x = size // 2 + 64
    coin_y = size // 2 + 20
    fill_circle(img, coin_x, coin_y, 30, (242, 195, 61, 255))
    fill_circle(img, coin_x, coin_y, 24, (255, 223, 120, 255))
    fill_circle(img, coin_x, coin_y, 10, (242, 195, 61, 255))

    # Flippers + feet
    fill_circle(img, size // 2 - 78, size // 2 + 24, 20, (22, 30, 42, 255))
    fill_circle(img, size // 2 + 52, size // 2 + 24, 20, (22, 30, 42, 255))
    fill_rect(img, size // 2 - 54, size // 2 + 96, size // 2 - 8, size // 2 + 116, (250, 173, 52, 255))
    fill_rect(img, size // 2 + 8, size // 2 + 96, size // 2 + 54, size // 2 + 116, (250, 173, 52, 255))

    write_png(path, size, size, img)


def generate_sword_png(path: str, size: int = 128) -> None:
    img = canvas(size, size)

    # Transparent bg + subtle glow circle
    fill_circle(img, size // 2, size // 2, size // 2 - 8, (48, 65, 96, 180))

    # Blade
    for i in range(14, 88):
        x = size // 2 + (i - 14) // 7
        y = i
        set_px(img, x - 2, y, (220, 235, 255, 255))
        set_px(img, x - 1, y, (235, 245, 255, 255))
        set_px(img, x, y, (255, 255, 255, 255))
        set_px(img, x + 1, y, (205, 220, 245, 255))

    # Tip
    fill_diamond(img, size // 2 + 10, 11, 7, 7, (235, 245, 255, 255))

    # Guard
    fill_rect(img, size // 2 - 20, 84, size // 2 + 26, 92, (255, 183, 66, 255))
    fill_circle(img, size // 2 - 20, 88, 4, (226, 143, 40, 255))
    fill_circle(img, size // 2 + 26, 88, 4, (226, 143, 40, 255))

    # Grip and pommel
    fill_rect(img, size // 2 + 1, 93, size // 2 + 7, 112, (122, 78, 47, 255))
    fill_circle(img, size // 2 + 4, 117, 6, (255, 183, 66, 255))

    write_png(path, size, size, img)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate Tuxium runtime PNG assets.")
    parser.add_argument("--app-icon", required=True, help="Output PNG path for app icon")
    parser.add_argument("--sword-icon", required=True, help="Output PNG path for sword icon")
    args = parser.parse_args()

    generate_app_icon(args.app_icon)
    generate_sword_png(args.sword_icon)


if __name__ == "__main__":
    main()
