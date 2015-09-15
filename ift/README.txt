- Dependencias: Primeiramente é importante instalar todas as dependencias necessárias ao funcionamento da libift. São elas:
	libsvm-tools liblapack-dev libblas-dev libatlas-base-dev
	Que podem ser instaladas via apt-get no ubuntu.

- Todos os programas disponíveis se encontram na pasta demo/
- Para compilar um programa, entre na pasta demo e digite no teminal:
    - make nome_do_programa_sem_a_extensao
    
    - P.ex: para compilar o programa iftExtractFeatures.c, faça:
        - make iftExtractFeatures

- Será gerado um arquivo binário na pasta bin/ com mesmo nome do programa compilado.

