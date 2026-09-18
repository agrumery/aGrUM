# -*- coding: utf-8 -*-

LEAVES = ["markov_random_field", "influence_diagram", "credal_net", "causal_model", "prm", "ktbn"]

# palette
CORE_FILL   = "#123C69"
CORE_TEXT   = "#FFFFFF"
EAGER_FILL  = "#1E6091"
EAGER_TEXT  = "#FFFFFF"
LAZY_FILL   = "#DCEAF6"
LAZY_BORDER = "#8FB4D9"
LAZY_TEXT   = "#3E6489"
CHIP_FILL   = "#2B2F36"
CHIP_BORDER = "#5A6270"
ARROW_EAGER = "#3D9BE9"
BUS_COLOR   = "#9AA5B1"
TITLE_COLOR = "#1B1F24"
SUB_COLOR   = "#5B6672"

FONT = "Helvetica, Arial, sans-serif"
MONO = "SFMono-Regular, Menlo, Consolas, monospace"

PROMPT_COLOR  = "#7F8A9B"
KEYWORD_COLOR = "#C586C0"
MODULE_COLOR  = "#4FC1FF"
CODE_SIZE = 15
CODE_CHAR_W = CODE_SIZE * 0.6

# every module box (core included) shares the same footprint; the whole
# import graph is now a single vertical column, to keep the figure narrow.
# Widened from the original 130/260 to comfortably fit the longest submodule
# names (markov_random_field, influence_diagram); fit_font_size() below is a
# safety net that shrinks a label's font further if it ever still overflows.
MODULE_W, MODULE_H = 220, 60
CHIP_W, CHIP_H = 380, 60
TITLEBAR_H = 18
TITLEBAR_FILL = "#3A3D41"
DOT_COLORS = ["#FF5F56", "#FFBD2E", "#27C93F"]

ARROW_GAP = 46      # chip -> core
STACK_GAP = 22      # between consecutive (lazy) boxes in the column
TAB_OVERLAP = 12    # merge overlap between core and an eager leaf right below it
BUS_OFFSET = 30      # horizontal distance from the column to the side bus

PW = MODULE_W + 2 * BUS_OFFSET + 70  # column + bus + stub + margins
PW = max(PW, CHIP_W + 40)

PANEL_TITLE_Y = 26
TOP_MARGIN = 20
SIDE_MARGIN = 50
GAP_BETWEEN = 60


def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def rounded_rect(x, y, w, h, rx, fill, stroke=None, stroke_width=1.5, dash=None):
    dash_attr = f' stroke-dasharray="{dash}"' if dash else ""
    stroke_attr = f' stroke="{stroke}" stroke-width="{stroke_width}"' if stroke else ""
    return f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" rx="{rx}" ry="{rx}" fill="{fill}"{stroke_attr}{dash_attr}/>'


def text(x, y, s, size=14, weight="normal", fill="#000", anchor="middle", family=FONT, italic=False):
    style = f'font-family="{family}" font-size="{size}" font-weight="{weight}" fill="{fill}" text-anchor="{anchor}"'
    if italic:
        style += ' font-style="italic"'
    return f'<text x="{x:.1f}" y="{y:.1f}" {style}>{esc(s)}</text>'


def fit_font_size(label, box_w, max_size=18, min_size=11, padding=20, char_w_ratio=0.62):
    """Box label font size that keeps `label` within `box_w` (bold Helvetica width estimate)."""
    available = box_w - padding
    size = available / (len(label) * char_w_ratio)
    return max(min_size, min(max_size, size))


def code_line(x, y, parts):
    """Left-anchored, syntax-highlighted monospace line: parts = [(text, color, weight), ...]."""
    spans = []
    cursor = x
    for s, color, weight in parts:
        spans.append(f'<tspan x="{cursor:.1f}" y="{y:.1f}" font-weight="{weight}" fill="{color}">{esc(s)}</tspan>')
        cursor += len(s) * CODE_CHAR_W
    return f'<text font-family="{MONO}" font-size="{CODE_SIZE}" xml:space="preserve">{"".join(spans)}</text>'


def panel(panel_x, chip_label, eager_leaf, panel_title, show_core_loaded=True):
    svg = []
    cx = PW / 2

    y = 10  # cursor

    # chip: a small terminal window (title bar with traffic-light dots + a
    # dark body) hosting the python-console code line -- makes the dark
    # cartouche read clearly as a terminal now that the page background is
    # transparent
    chip_x0 = cx - CHIP_W / 2
    svg.append(rounded_rect(chip_x0, y, CHIP_W, CHIP_H, 9, CHIP_FILL, stroke=CHIP_BORDER, stroke_width=1.4))
    svg.append(rounded_rect(chip_x0, y, CHIP_W, TITLEBAR_H, 9, TITLEBAR_FILL))
    for i, dot_color in enumerate(DOT_COLORS):
        svg.append(f'<circle cx="{chip_x0 + 14 + i * 13:.1f}" cy="{y + TITLEBAR_H / 2:.1f}" r="4" fill="{dot_color}"/>')

    body_cy = y + TITLEBAR_H + (CHIP_H - TITLEBAR_H) / 2
    module_name = chip_label.split(" ", 1)[1]
    code_parts = [(">>> ", PROMPT_COLOR, "400"), ("import", KEYWORD_COLOR, "700"), (" " + module_name, MODULE_COLOR, "600")]
    CODE_LEFT_PAD = 16
    svg.append(code_line(chip_x0 + CODE_LEFT_PAD, body_cy + 5, code_parts))
    y += CHIP_H

    # arrow: chip -> core (always eager)
    arrow_y1 = y
    y += ARROW_GAP
    core_top = y
    svg.append(
        f'<line x1="{cx:.1f}" y1="{arrow_y1:.1f}" x2="{cx:.1f}" y2="{core_top - 2:.1f}" '
        f'stroke="{ARROW_EAGER}" stroke-width="2.6" marker-end="url(#arrow-eager)"/>'
    )
    bus_x = cx - MODULE_W / 2 - BUS_OFFSET
    label_y = (arrow_y1 + core_top) / 2 + 4
    svg.append(text(cx + 14, label_y, "eager", size=11.5, weight="600",
                     fill=ARROW_EAGER, anchor="start"))
    svg.append(text(bus_x + 8, label_y, "lazy", size=11.5, weight="600",
                     fill=BUS_COLOR, anchor="start"))
    jog_y = arrow_y1 + 10

    # core box
    core_cy = core_top + MODULE_H / 2
    svg.append(rounded_rect(cx - MODULE_W / 2, core_top, MODULE_W, MODULE_H, 10, CORE_FILL))
    if show_core_loaded:
        svg.append(text(cx, core_cy - 11, "core", size=18, weight="700", fill=CORE_TEXT))
        svg.append(text(cx, core_cy + 4, "BASE + BN", size=10, fill="#CBD9E8"))
        svg.append(text(cx, core_cy + 17, "loaded", size=10, weight="600", fill="#D7E9F2"))
    else:
        svg.append(text(cx, core_cy - 4, "core", size=18, weight="700", fill=CORE_TEXT))
        svg.append(text(cx, core_cy + 12, "BASE + BN", size=10, fill="#CBD9E8"))
    y = core_top + MODULE_H

    # bus: runs down the side of the column, from a jog under the chip to the
    # last lazy box; short stubs reach into each lazy box
    bus_x = cx - MODULE_W / 2 - BUS_OFFSET
    bus_top = jog_y

    stub_lines = []  # collected while walking the column, drawn after the bus itself
    lazy_boxes = []

    for i, name in enumerate(LEAVES):
        is_eager = (name == eager_leaf)
        if is_eager and i == 0:
            # merged directly onto core: overlap, same visual family, no bus stub
            top = y - TAB_OVERLAP
            svg.append(rounded_rect(cx - MODULE_W / 2, top, MODULE_W, MODULE_H, 10, EAGER_FILL))
            svg.append(text(cx, top + MODULE_H / 2 - 3, name, size=fit_font_size(name, MODULE_W), weight="700", fill=EAGER_TEXT))
            svg.append(text(cx, top + MODULE_H / 2 + 13, "loaded", size=10, weight="600", fill="#D7E9F2"))
            y = top + MODULE_H
        else:
            top = y + STACK_GAP
            box_cy = top + MODULE_H / 2
            lazy_boxes.append((name, top, box_cy))
            stub_lines.append(box_cy)
            y = top + MODULE_H

    bus_bottom = stub_lines[-1] if stub_lines else bus_top
    svg.append(
        f'<path d="M {cx - 6:.1f} {arrow_y1:.1f} '
        f'L {cx - 6:.1f} {jog_y:.1f} '
        f'L {bus_x:.1f} {jog_y:.1f} '
        f'L {bus_x:.1f} {bus_bottom:.1f}" '
        f'fill="none" stroke="{BUS_COLOR}" stroke-width="1.8"/>'
    )

    for name, top, box_cy in lazy_boxes:
        svg.append(rounded_rect(cx - MODULE_W / 2, top, MODULE_W, MODULE_H, 9, LAZY_FILL, stroke=LAZY_BORDER,
                                 stroke_width=1.6))
        svg.append(text(cx, box_cy - 3, name, size=fit_font_size(name, MODULE_W), weight="700", fill=LAZY_TEXT))
        svg.append(text(cx, box_cy + 13, "lazy", size=10, weight="600", italic=True, fill=LAZY_BORDER))
        svg.append(
            f'<line x1="{bus_x:.1f}" y1="{box_cy:.1f}" x2="{cx - MODULE_W/2 - 2:.1f}" y2="{box_cy:.1f}" '
            f'stroke="{BUS_COLOR}" stroke-width="1.8" marker-end="url(#arrow-lazy)"/>'
        )

    panel_height = y
    return "\n".join(svg), panel_height


defs = f'''
<defs>
  <marker id="arrow-eager" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
    <path d="M 0 0 L 10 5 L 0 10 z" fill="{ARROW_EAGER}"/>
  </marker>
  <marker id="arrow-lazy" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
    <path d="M 0 0 L 10 5 L 0 10 z" fill="{BUS_COLOR}"/>
  </marker>
</defs>
'''

panel1_x = SIDE_MARGIN
panel2_x = SIDE_MARGIN + PW + GAP_BETWEEN

svg1, h1 = panel(panel1_x, "import pyagrum", None, "import pyagrum")
svg2, h2 = panel(panel2_x, "import pyagrum.markov_random_field", "markov_random_field", "import pyagrum.markov_random_field", show_core_loaded=False)
panel_h = max(h1, h2)

body = []
body.append(f'<g transform="translate({panel1_x},{TOP_MARGIN})">{svg1}</g>')
body.append(f'<g transform="translate({panel2_x},{TOP_MARGIN})">{svg2}</g>')

TOTAL_W = panel2_x + PW + SIDE_MARGIN
TOTAL_H = TOP_MARGIN + panel_h + 30

svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="{TOTAL_W:.0f}" height="{TOTAL_H:.0f}" viewBox="0 0 {TOTAL_W:.0f} {TOTAL_H:.0f}">
{defs}
{"".join(body)}
</svg>'''

with open("/tmp/pyagrum_diagram/import_paths.svg", "w") as f:
    f.write(svg)

print("width", TOTAL_W, "height", TOTAL_H)
