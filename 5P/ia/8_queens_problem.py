def hill_climbing_8_queens():
    estado_atual = gerar_estado_aleatorio()  # Vetor de 8 posições
    while True:
        conflitos_atuais = calcular_conflitos(estado_atual)
        if conflitos_atuais == 0:
            return estado_atual  # Solução encontrada
        vizinhos = gerar_vizinhos(estado_atual)  # Mover uma rainha por vez
        melhor_vizinho, melhor_conflitos = encontrar_melhor_vizinho(vizinhos)
        if melhor_conflitos >= conflitos_atuais:
          return None  # Máximo local, reinicie ou pare
        estado_atual = melhor_vizinho

def encontrar_melhor_vizinho(vizinhos):
    melhor_vizinho = None
    melhor_conflitos = float('inf')
    for vizinho in vizinhos:
        conflitos = calcular_conflitos(vizinho)
        if conflitos < melhor_conflitos:
            melhor_conflitos = conflitos
            melhor_vizinho = vizinho
    return melhor_vizinho, melhor_conflitos

def calcular_conflitos(estado):
    # Conta o número de pares de rainhas em conflito
    conflitos = 0
    for i in range(len(estado)):
        for j in range(i + 1, len(estado)):
            if estado[i] == estado[j] or abs(estado[i] - estado[j]) == abs(i - j):
                conflitos += 1
    return conflitos

def gerar_vizinhos(estado):
    # Gera todos os vizinhos possíveis movendo uma rainha para outra linha na mesma coluna
    vizinhos = []
    n = len(estado)
    for col in range(n):
        for row in range(n):
            if estado[col] != row:
                novo_estado = list(estado)
                novo_estado[col] = row
                vizinhos.append(novo_estado)
    return vizinhos

import random

def gerar_estado_aleatorio():
    # Gera um vetor de 8 posições, cada uma representando a linha da rainha na coluna correspondente
    return [random.randint(0, 7) for _ in range(8)]
