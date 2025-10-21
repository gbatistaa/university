from bs4 import BeautifulSoup

# Lista de arquivos SVG enviados
svg_files = [
    ("location-dot-solid-full.svg", "file-2XxG8Q82LZxZbPThL5cF8y"),
    ("suitcase-solid-full.svg", "file-QctxXaTvEqjj9b8uyNyyuD"),
    ("square-linkedin-brands-solid-full.svg", "file-VaGF3MG2hzuDPssXYhAD61"),
    ("envelope-solid-full.svg", "file-T2ZL8qbnH7DcDV1w9JPRVD"),
    ("phone-solid-full.svg", "file-QXXCFmkZCUofeBFCzHPD3P"),
    ("github-brands-solid-full.svg", "file-9T4f2nx99DvaRAbxioeKx6"),
]

# Função para alterar o fill de um SVG para branco
def update_svg_fill(svg_content: str) -> str:
    soup = BeautifulSoup(svg_content, "xml")
    for tag in soup.find_all(True):
        if tag.has_attr("fill"):
            tag["fill"] = "#FFFFFF"
    return str(soup)

# Processar todos os arquivos SVG
updated_svgs = []
for filename, file_id in svg_files:
    input_path = f"./{filename}"
    with open(input_path, "r", encoding="utf-8") as f:
        original_svg = f.read()
    modified_svg = update_svg_fill(original_svg)

    output_filename = filename.replace(".svg", "-white.svg")
    output_path = f"./icons/{output_filename}"
    with open(output_path, "w", encoding="utf-8") as f_out:
        f_out.write(modified_svg)

    updated_svgs.append(output_path)

updated_svgs
