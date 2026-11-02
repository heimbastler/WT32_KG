import os
import textwrap


PAGE_W, PAGE_H = 841.89, 595.28
OPS = []


def color(rgb, stroke=False):
    operator = "RG" if stroke else "rg"
    OPS.append("%.3f %.3f %.3f %s" % (*rgb, operator))


def rect(x, top, width, height, fill=None, stroke=None, line_width=0.5):
    y = PAGE_H - top - height
    OPS.append("q")
    if fill:
        color(fill)
    if stroke:
        color(stroke, True)
        OPS.append("%.2f w" % line_width)
    OPS.append("%.2f %.2f %.2f %.2f re %s" % (x, y, width, height, "B" if fill and stroke else "f" if fill else "S"))
    OPS.append("Q")


def line(x1, top1, x2, top2, rgb=(0.82, 0.86, 0.87), width=0.45):
    OPS.extend(["q", "%.3f %.3f %.3f RG" % rgb, "%.2f w" % width,
                "%.2f %.2f m %.2f %.2f l S" % (x1, PAGE_H - top1, x2, PAGE_H - top2), "Q"])


def draw_text(x, baseline_top, text, size=8, bold=False, rgb=(0.09, 0.13, 0.17)):
    encoded = text.encode("cp1252", errors="replace").hex().upper()
    font = "F2" if bold else "F1"
    OPS.extend(["q", "%.3f %.3f %.3f rg" % rgb,
                "BT /%s %.2f Tf 1 0 0 1 %.2f %.2f Tm <%s> Tj ET" %
                (font, size, x, PAGE_H - baseline_top, encoded), "Q"])


def wrap(text, max_width, size):
    words = text.split()
    lines = []
    current = ""
    max_chars = max(1, int(max_width / (size * 0.49)))
    for word in words:
        candidate = word if not current else current + " " + word
        if len(candidate) <= max_chars:
            current = candidate
        else:
            if current:
                lines.append(current)
            current = textwrap.fill(word, width=max_chars, break_long_words=True).splitlines()[0]
            if len(word) > max_chars:
                parts = textwrap.wrap(word, width=max_chars, break_long_words=True, break_on_hyphens=False)
                lines.extend(parts[:-1])
                current = parts[-1]
    if current:
        lines.append(current)
    return lines


def draw_paragraph(x, baseline_top, text, width, size=8, leading=10, bold=False, rgb=(0.09, 0.13, 0.17)):
    lines = wrap(text, width, size)
    for index, value in enumerate(lines):
        draw_text(x, baseline_top + index * leading, value, size=size, bold=bold, rgb=rgb)
    return baseline_top + len(lines) * leading


def draw_table(x, top, widths, headers, rows, row_heights, font_size=7.4, header_size=7.4):
    total_width = sum(widths)
    header_height = 18
    rect(x, top, total_width, header_height, fill=(0.90, 0.95, 0.93), stroke=(0.76, 0.82, 0.82))
    cell_x = x
    for index, title in enumerate(headers):
        draw_paragraph(cell_x + 4, top + 12, title, widths[index] - 8, size=header_size,
                       leading=8.5, bold=True)
        if index:
            line(cell_x, top, cell_x, top + header_height + sum(row_heights), width=0.35)
        cell_x += widths[index]
    current_top = top + header_height
    for row, height in zip(rows, row_heights):
        rect(x, current_top, total_width, height, fill=(1, 1, 1), stroke=(0.82, 0.86, 0.87), line_width=0.35)
        cell_x = x
        for index, value in enumerate(row):
            lines = wrap(value, widths[index] - 8, font_size)
            leading = font_size + 1.4
            start = current_top + max(font_size + 2, (height - len(lines) * leading) / 2 + font_size)
            for line_index, value_line in enumerate(lines):
                draw_text(cell_x + 4, start + line_index * leading, value_line, size=font_size)
            cell_x += widths[index]
        current_top += height
    return current_top


def pdf_escape_stream(data):
    return data.encode("ascii")


def build_pdf(image_path, output_path):
    margin = 28
    draw_text(margin, 30, "WT32-KG | Schaltschrank-Aushang", size=20, bold=True)
    draw_text(margin, 46, "Steuerung: WT32-ETH01   |   Netzwerk: Ethernet   |   Stand: 01.11.2026",
              size=8.5, rgb=(0.31, 0.38, 0.42))

    warning = ("Wichtiger Codeabgleich: Die vor Ort abgelesene Relaisliste stimmt bei den Rollläden nicht mit der aktuellen Firmware überein. "
               "R00/R01 und R02/R03 sind in der Firmware jeweils in umgekehrter Fahrtrichtung benannt und angesteuert. "
               "R12 ist laut Schaltschrankliste Kronleuchter L2 (nicht angeschlossen), im Code aber Reserve Wohnzimmer und als "
               "Home-Assistant-Light aktiviert. Vor Bedienung dieser Ausgänge Firmware und reale Verdrahtung abgleichen.")
    rect(margin, 56, PAGE_W - margin * 2, 55, fill=(1.0, 0.97, 0.95), stroke=(0.74, 0.25, 0.21), line_width=1)
    rect(margin, 56, 4, 55, fill=(0.74, 0.25, 0.21))
    draw_paragraph(margin + 13, 72, warning, PAGE_W - margin * 2 - 28, size=8.1, leading=10.4)

    left_x, left_w = margin, 270
    right_x, right_w = 312, PAGE_W - margin - 312
    draw_text(left_x, 132, "Weboberfläche", size=11, bold=True)
    draw_text(left_x, 151, "IP: http://192.168.178.195/", size=8.1, bold=True)
    draw_text(left_x, 164, "mDNS: http://wt32-kg.local/", size=8.1, bold=True)
    draw_text(left_x, 177, "Repository:", size=8.1, bold=True)
    draw_text(left_x, 190, "github.com/heimbastler/WT32_KG", size=7.4, bold=True, rgb=(0.09, 0.42, 0.34))

    image = open(image_path, "rb").read()
    image_x, image_top, image_size = left_x + left_w - 75, 141, 68
    draw_text(image_x - 2, image_top + image_size + 10, "Repository-QR", size=6.5, rgb=(0.31, 0.38, 0.42))

    draw_text(left_x, 222, "MQTT / Home Assistant", size=11, bold=True)
    mqtt_rows = [
        ("MQTT", "In der Weboberfläche unter MQTT aktivieren"),
        ("Broker", "192.168.178.1 (Firmware-Standard; gespeicherten Wert prüfen)"),
        ("Port", "1883 (Firmware-Standard)"),
        ("Benutzer", "________________________________"),
        ("Passwort", "________________________________"),
        ("Transport", "Ethernet / TCP"),
        ("Base Topic", "wt32kg"),
        ("HA Discovery Prefix", "homeassistant"),
    ]
    mqtt_heights = [19, 26, 19, 19, 19, 19, 19, 19]
    mqtt_bottom = draw_table(left_x, 231, [89, left_w - 89], ["Einstellung", "Wert"], mqtt_rows,
                             mqtt_heights, font_size=7.2, header_size=7.2)
    draw_paragraph(left_x, mqtt_bottom + 14,
                   "Broker, Port und Zugangsdaten können über die MQTT-Seite geändert werden und werden im ESP32 gespeichert. "
                   "Das Passwort ist absichtlich nicht auf diesem Blatt hinterlegt.", left_w, size=7.1, leading=9,
                   rgb=(0.31, 0.38, 0.42))

    draw_text(right_x, 132, "Relaiszuordnung laut Schaltschrank", size=11, bold=True)
    relay_rows = [
        ("R00", "<044>", "Fensterjalousie runter"),
        ("R01", "<047>", "Fensterjalousie hoch"),
        ("R02", "<045>", "Türjalousie runter"),
        ("R03", "<046>", "Türjalousie hoch"),
        ("R04", "<043>", "Außenlampe Garten"),
        ("R05", "<019>", "Steinlampen / Stein"),
        ("R06", "<120>", "KG-Flur"),
        ("R07", "<025>", "Arbeitsleuchten"),
        ("R08", "<011>", "Küche"),
        ("R09", "<010>", "EG-Flur"),
        ("R10", "<009>", "Trägerlampen / Tisch"),
        ("R11", "<033>", "Kronleuchter"),
        ("R12", "<034>", "Kronleuchter L2, nicht an Lampe angeschlossen"),
        ("R13–R15", "—", "In RELAIS.md nicht zugeordnet"),
        ("R16–R23", "—", "In RELAIS.md nicht aufgeführt"),
    ]
    relay_bottom = draw_table(right_x, 141, [62, 63, right_w - 125],
                              ["Relais", "Kabel", "Physische Funktion laut RELAIS.md"], relay_rows,
                              [14] * len(relay_rows), font_size=7.1, header_size=7.0)
    firmware_title_top = relay_bottom + 17
    draw_text(right_x, firmware_title_top, "Firmwareabweichungen", size=10, bold=True)
    firmware_rows = [
        ("R00 / R01", "R00 runter, R01 hoch", "R00 hoch, R01 runter; Cover 0 nutzt R00 als open"),
        ("R02 / R03", "R02 runter, R03 hoch", "R02 hoch, R03 runter; Cover 1 nutzt R02 als open"),
        ("R07", "Arbeitsleuchten", "Küchenarbeitslampe"),
        ("R12", "Kronleuchter L2, nicht angeschlossen", "Reserve Wohnzimmer; HA-Light standardmäßig aktiviert"),
    ]
    firmware_top = firmware_title_top + 9
    firmware_bottom = draw_table(right_x, firmware_top, [70, 154, right_w - 224],
                                 ["Relais", "Schaltschrankliste", "Aktueller Code in src/main.cpp"], firmware_rows,
                                 [25, 25, 19, 25], font_size=6.8, header_size=6.8)
    draw_paragraph(right_x, firmware_bottom + 12,
                   "PCA9535: Board A 0x22 = R00–R07, Board B 0x23 = R08–R15, Board C 0x24 = R16–R23.",
                   right_w, size=6.8, leading=8, rgb=(0.31, 0.38, 0.42))

    commands = "\n".join(OPS).encode("ascii")
    jpeg = open(image_path, "rb").read()
    objects = [
        b"<< /Type /Catalog /Pages 2 0 R >>",
        b"<< /Type /Pages /Kids [3 0 R] /Count 1 >>",
        ("<< /Type /Page /Parent 2 0 R /MediaBox [0 0 %.2f %.2f] "
         "/Resources << /Font << /F1 4 0 R /F2 5 0 R >> /XObject << /Im1 6 0 R >> >> "
         "/Contents 7 0 R >>" % (PAGE_W, PAGE_H)).encode("ascii"),
        b"<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica /Encoding /WinAnsiEncoding >>",
        b"<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold /Encoding /WinAnsiEncoding >>",
        (b"<< /Type /XObject /Subtype /Image /Width 320 /Height 320 /ColorSpace /DeviceRGB "
         b"/BitsPerComponent 8 /Filter /DCTDecode /Length %d >>\nstream\n" % len(jpeg)
         + jpeg + b"\nendstream"),
        b"<< /Length %d >>\nstream\n" % len(commands) + commands + b"\nendstream",
    ]
    pdf = bytearray(b"%PDF-1.4\n%\xe2\xe3\xcf\xd3\n")
    offsets = [0]
    for number, body in enumerate(objects, 1):
        offsets.append(len(pdf))
        pdf.extend(b"%d 0 obj\n" % number + body + b"\nendobj\n")
    xref_offset = len(pdf)
    pdf.extend(b"xref\n0 %d\n" % (len(objects) + 1))
    pdf.extend(b"0000000000 65535 f \n")
    for offset in offsets[1:]:
        pdf.extend(b"%010d 00000 n \n" % offset)
    pdf.extend(b"trailer\n<< /Size %d /Root 1 0 R >>\nstartxref\n%d\n%%%%EOF\n" %
               (len(objects) + 1, xref_offset))
    with open(output_path, "wb") as pdf_file:
        pdf_file.write(pdf)


build_pdf("/tmp/wt32kg-repo-qr.jpg", "SCHALTSCHRANK_AUSHANG.pdf")
print("Created SCHALTSCHRANK_AUSHANG.pdf")