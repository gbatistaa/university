# ExemplosMPI

## Instalação do MPICH no Ubuntu

```bash
sudo apt install mpich
```

## Exemplo de compilação

```bash
mpicc mpi_hello.c -o mpihello
```

## Exemplo de execução

```bash
mpiexec -n 4 ./mpihello
```
