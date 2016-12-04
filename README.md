# t2fs.h

## Segundo trabalho de INF01142 - Sistemas Operacionais I

O objetivo desse trabalho é a aplicação dos conceitos de sistemas operacionais na implementação de um Sistema de Arquivos que empregue alocação indexada combinada para criação de arquivos e diretórios.

## Makefiles

### Testes

- Criar `teste/test_<arq>.c`, `include/<arq>.h` e `src/<arq.c>` (ordem recomendada pelo TDD, não que eu siga).
- Incluir `<arq>` na variável `FILES` de `Makefile`.
- Incluir `<arq>` na variável `TEST` de `Makefile`.
- Rodar `make test | grep -v gcc | grep -v rm` para ter somente os warnings e os erros gerados pelos testes.
- Rodar `make test` para ter saída completa.

### Ordem para fazer as coisas:

- Utilizar `make -f makeord`.
