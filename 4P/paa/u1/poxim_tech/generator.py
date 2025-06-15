import random

def gerar_cnpj():
    """Gera um CNPJ aleatório no formato XX.XXX.XXX/YYYY-ZZ"""
    bloco1 = random.randint(10, 99)
    bloco2 = random.randint(100, 999)
    bloco3 = random.randint(100, 999)
    bloco4 = random.randint(1000, 9999)
    digito = random.randint(10, 99)
    return f"{bloco1:02}.{bloco2:03}.{bloco3:03}/{bloco4:04}-{digito:02}"

def gerar_codigo_container():
    """Gera um código de container aleatório no formato XXXX1234567"""
    letras = ''.join(random.choices("ABCDEFGHIJKLMNOPQRSTUVWXYZ", k=4))
    numeros = ''.join(random.choices("0123456789", k=7))
    return f"{letras}{numeros}"

def gerar_modelo_input(qtd_containers, qtd_inspecoes, peso_min=10000, peso_max=30000):
    """
    Gera o modelo de input.
    :param qtd_containers: Número de containers no registro base.
    :param qtd_inspecoes: Número de containers a serem inspecionados.
    :param peso_min: Peso mínimo do container.
    :param peso_max: Peso máximo do container.
    :return: String no formato esperado.
    """
    containers = []
    inspecoes = []

    # Gera containers no registro base
    for _ in range(qtd_containers):
        codigo = gerar_codigo_container()
        cnpj = gerar_cnpj()
        peso = random.randint(peso_min, peso_max)
        containers.append((codigo, cnpj, peso))

    # Gera containers inspecionados
    for _ in range(qtd_inspecoes):
        codigo, cnpj, peso_base = random.choice(containers)
        peso_inspecao = random.randint(peso_min, peso_max)

        # Às vezes altera o CNPJ para gerar um erro proposital
        if random.random() < 0.3:  # 30% de chance de erro no CNPJ
            cnpj = gerar_cnpj()

        inspecoes.append((codigo, cnpj, peso_inspecao))

    # Monta o modelo como uma string
    modelo = [str(len(containers))]
    for codigo, cnpj, peso in containers:
        modelo.append(f"{codigo} {cnpj} {peso}")

    modelo.append(str(len(inspecoes)))
    for codigo, cnpj, peso in inspecoes:
        modelo.append(f"{codigo} {cnpj} {peso}")

    return "\n".join(modelo)

# Salvar o modelo em um arquivo
def salvar_modelo_em_arquivo(filename, qtd_containers, qtd_inspecoes):
    modelo = gerar_modelo_input(qtd_containers, qtd_inspecoes)
    with open(filename, "w") as f:
        f.write(modelo)
    print(f"Modelo de input salvo em: {filename}")

# Exemplo de uso
if __name__ == "__main__":
    salvar_modelo_em_arquivo("input.txt", qtd_containers=50000, qtd_inspecoes=10000)
