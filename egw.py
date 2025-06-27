from fpdf import FPDF
from fpdf.enums import XPos, YPos

class PDF(FPDF):
    def header(self):
        self.set_font("Helvetica", "B", 14)
        self.cell(0, 10, "Livros de Ellen G. White: Escritos Diretos vs Compilações",
                  new_x=XPos.LMARGIN, new_y=YPos.NEXT, align="C")
        self.ln(5)

    def chapter_title(self, title):
        self.set_font("Helvetica", "B", 12)
        self.cell(0, 10, title, new_x=XPos.LMARGIN, new_y=YPos.NEXT, align="L")
        self.ln(2)

    def chapter_body(self, items):
        self.set_font("Helvetica", "", 11)
        for item in items:
            self.multi_cell(w=0, h=8, txt=f"- {item}", new_x=XPos.LMARGIN, new_y=YPos.NEXT)
        self.ln(4)

pdf = PDF()
pdf.set_auto_page_break(auto=True, margin=15)
pdf.add_page()

# Texto de introdução
pdf.set_font("Helvetica", "", 11)
intro = (
    "Esta lista apresenta uma classificação dos livros atribuídos a Ellen G. White, "
    "dividindo-os entre aqueles que foram escritos diretamente por ela e os que são "
    "compilações feitas por terceiros a partir de seus escritos, geralmente após sua morte, "
    "sob responsabilidade da White Estate."
)
pdf.multi_cell(w=0, h=8, txt=intro, new_x=XPos.LMARGIN, new_y=YPos.NEXT)
pdf.ln(5)

# Listas
escritos = [
    "O Grande Conflito (The Great Controversy)",
    "Caminho a Cristo (Steps to Christ)",
    "Patriarcas e Profetas (Patriarchs and Prophets)",
    "Profetas e Reis (Prophets and Kings)",
    "O Desejado de Todas as Nações (The Desire of Ages)",
    "Atos dos Apóstolos (The Acts of the Apostles)",
    "A Ciência do Bom Viver (The Ministry of Healing)",
    "Educação (Education)",
    "Primeiros Escritos (Early Writings)",
    "Testemunhos para a Igreja (Testimonies for the Church) - Vol. 1 a 9",
    "O Maior Discurso de Cristo (Thoughts from the Mount of Blessing)",
    "Parábolas de Jesus (Christ’s Object Lessons)",
    "Fundamentos da Educação Cristã (Fundamentals of Christian Education)",
]

compilados = [
    "Mensagens aos Jovens (Messages to Young People)",
    "Lar Adventista (The Adventist Home)",
    "Mente, Caráter e Personalidade (Mind, Character, and Personality) - Vol. 1 e 2",
    "Conselhos Sobre Regime Alimentar (Counsels on Diet and Foods)",
    "Conselhos Sobre Mordomia (Counsels on Stewardship)",
    "Conselhos aos Professores, Pais e Estudantes (Counsels to Parents, Teachers, and Students)",
    "Temperança (Temperance)",
    "Evangelismo (Evangelism)",
    "Serviço Cristão (Christian Service)",
    "Medicina e Salvação (Medical Ministry)",
    "Obreiros Evangélicos (Gospel Workers - edição de 1915)",
    "Mensagens Escolhidas - Volumes 1, 2 e 3 (Selected Messages)",
    "O Evangelismo no Poder do Espírito (Evangelism in the Power of the Spirit)",
    "O Colportor Evangelista (Colporteur Ministry)",
    "O Líder Cristão (Christian Leadership)",
    "Testemunhos para Ministros (Testimonies to Ministers and Gospel Workers) - compilação parcial",
]

pdf.chapter_title("Livros Escritos Diretamente por Ellen G. White")
pdf.chapter_body(escritos)

pdf.chapter_title("Livros de Compilação (Não Escritos Como Obras Completas por Ellen White)")
pdf.chapter_body(compilados)

pdf.output("Livros_Ellen_White_Escritos_vs_Compilacoes.pdf")
