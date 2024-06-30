#include <stdio.h>
#include <stdlib.h>


#define MAX_MEM_FISICA 1024 // Tamanho da memória física em bytes
#define TAM_PAGINA 32       // Tamanho de cada página em bytes
#define TAM_MAX_PROCESSO 128 // Tamanho máximo de um processo em bytes
#define TOTAL_QUADROS MAX_MEM_FISICA / TAM_PAGINA

unsigned char memoria_fisica[MAX_MEM_FISICA];    // Memória física
int quadros_livres[MAX_MEM_FISICA / TAM_PAGINA]; // Lista de quadros livres

// Estrutura para a tabela de páginas
struct TabelaPaginas {
    int num_processo;
	int paginas_necessarias;
	int paginas[TOTAL_QUADROS];
};

// Estrutura para armazenar múltiplas tabelas de páginas
struct ListaTabelasPaginas {
    struct TabelaPaginas *tabelas;
    int count;    // Contador de quantas tabelas estão armazenadas
} lista_tabelas;  // Variável global para armazenar as tabelas de páginas

// Função para inicializar a lista de quadros livres
void inicializar_quadros_livres() {
    for (int i = 0; i < MAX_MEM_FISICA / TAM_PAGINA; ++i) {
        quadros_livres[i] = 1;                                // 1 indica que o quadro está livre
    }
}

// Função para exibir a memória física e seu percentual de livre
void visualizar_memoria() {
    int livres = 0;
	int total_quadros = TOTAL_QUADROS;

    // Contagem de quadros livres
    for (int i = 0; i < TOTAL_QUADROS; ++i) {
        if (quadros_livres[i] == 1) {
            livres++;
        }
    }
	
    // Exibição do percentual de memória livre
    printf("\nMemoria fisica: %d%% livre\n", (livres * 100) / total_quadros);

    printf("--------------------------------\n");
    // Exibição de cada quadro e seu conteúdo
    for (int i = 0; i < TOTAL_QUADROS; ++i) {
        printf("Quadro %02d: ", i);
        if (quadros_livres[i] == 0) {
            for (int j = 0; j < TAM_PAGINA; ++j) {
				if (j == TAM_PAGINA - 1) {
					printf("Processo %d", memoria_fisica[i * TAM_PAGINA + j]);
				}
            }
        } else {
            printf("Livre");
        }
        printf("\n");
    }
    printf("--------------------------------\n");
}

// Função para criar um novo processo e adicionar à lista de processos
void criar_processo(int num_novo_processo, int tamanho_processo) {
	if(lista_tabelas.count > 0) {
		for (int i = 0; i < lista_tabelas.count; ++i) {
			if (num_novo_processo == lista_tabelas.tabelas[i].num_processo) {
				printf("Processo numero %d ja existe na memoria.\n", num_novo_processo);
				return;
			}
		}
	}
	
    if (tamanho_processo > TAM_MAX_PROCESSO) {
        printf("> Tamanho do processo excede o maximo configurado de %d.\n", TAM_MAX_PROCESSO);
        return;
    }

    // Páginas necessárias para alocar o processo.
    int paginas_necessarias = (tamanho_processo + TAM_PAGINA - 1) / TAM_PAGINA; 
    int quadros_necessarios = 0;

    // Verifica se há memória suficiente para alocar o processo
    for (int i = 0; i < MAX_MEM_FISICA / TAM_PAGINA; ++i) {
        if (quadros_livres[i] == 1) {
            quadros_necessarios++;
            if (quadros_necessarios == paginas_necessarias) {
                break;
            }
        }
    }

    // Verifica se há quadros suficientes para alocar as páginas.
    if (quadros_necessarios < paginas_necessarias) {
        printf("> Memoria fisica insuficiente para alocar o processo.\n");
        return;
    }

    struct TabelaPaginas tabela_paginas; // Cria a estrutura para um novo processo.
	tabela_paginas.num_processo = num_novo_processo; // Seta o numero do processo.
	tabela_paginas.paginas_necessarias = paginas_necessarias; // Seta a quantidade de paginas necessárias.
	
	int n_pagina = 0; // Variável auxiliar para contar as páginas.

    // Alocar páginas para o processo
    for (int i = 0; i < MAX_MEM_FISICA / TAM_PAGINA; ++i) {
		if (n_pagina < paginas_necessarias) {
			if (quadros_livres[i] == 1) {
				// Alocar quadro
				quadros_livres[i] = 0; // Marca o quadro como ocupado.
				tabela_paginas.paginas[n_pagina] = i; // Marca o numero quadro na tabela de paginas do processo.

				// Inicializar a página com o número do processo
				for (int j = 0; j < TAM_PAGINA; ++j) {
					memoria_fisica[i * TAM_PAGINA + j] = num_novo_processo;
				}
				n_pagina++;
			}
		} else {
			tabela_paginas.paginas[i] = -1;
		}
    }
	
    // Adicionar tabela de páginas à lista global
    lista_tabelas.tabelas[lista_tabelas.count++] = tabela_paginas;

    // Exibir tabela de páginas do processo criado
    printf("\n----------------------------------\n");
    printf("Processo %d criado com sucesso:\n", num_novo_processo);
    printf("Tamanho do processo: %d bytes\n", tamanho_processo);
    printf("Tabela de páginas:\n");
    for (int i = 0; i < paginas_necessarias; ++i) {
        printf("Pagina %d -> Quadro %d\n", i, tabela_paginas.paginas[i]);
    }
    printf("\n----------------------------------\n");
}

void visualizar_tabela_paginas(int num_processo) {
    // Buscar a tabela de páginas correspondente ao processo
    for (int i = 0; i < lista_tabelas.count; ++i) {
        if (lista_tabelas.tabelas[i].num_processo == num_processo) {
			
            printf("\nTabela de paginas do processo: %d\n", num_processo);
            for (int j = 0; j < lista_tabelas.tabelas[i].paginas_necessarias; ++j) {
				printf("Pagina %d -> Quadro %d\n", j, lista_tabelas.tabelas[i].paginas[j]);
            }
            return;
        }
    }
    printf("> Processo nao encontrado.\n");
}

// Função para visualizar todos os processos criados
void visualizar_processos() {
    printf("\nTabela de processos:\n");
    for (int i = 0; i < lista_tabelas.count; ++i) {
        printf("Numero do processo: %d, Paginas necessarias: %d\n", lista_tabelas.tabelas[i].num_processo, lista_tabelas.tabelas[i].paginas_necessarias);
    }
}

void liberar_memoria_fisica(int num_processo) {
    for (int i = 0; i < TOTAL_QUADROS; ++i) {
        if (quadros_livres[i] == 0) {
            for (int j = 0; j < TAM_PAGINA; ++j) {
                if (num_processo == memoria_fisica[i * TAM_PAGINA + j]) {
                    quadros_livres[i] = 1;  // Marca o quadro como livre na memória física

                    // Remover completamente a entrada correspondente na lista_tabelas
                    for (int k = 0; k < lista_tabelas.count; ++k) {
                        if (lista_tabelas.tabelas[k].num_processo == num_processo) {
                            // Liberar todas as páginas associadas ao processo
                            for (int l = 0; l < lista_tabelas.tabelas[k].paginas_necessarias; ++l) {
                                int quadro = lista_tabelas.tabelas[k].paginas[l];
                                quadros_livres[quadro] = 1; // Marca o quadro como livre na memória física
                            }

                            // Remover completamente a entrada da lista_tabelas
                            for (int m = k; m < lista_tabelas.count - 1; ++m) {
                                lista_tabelas.tabelas[m] = lista_tabelas.tabelas[m + 1];
                            }
                            lista_tabelas.count--;
							printf("\n> Processo %d removido com sucesso!\n", num_processo);
                            return;
                        }
                    }
                }
            }
        } 
    }
	printf("\n> Processo nao encontrato\n");
}

// Função para inicializar a lista de tabelas de páginas
void inicializar_lista_tabelas() {
    lista_tabelas.tabelas = (struct TabelaPaginas *)malloc(TOTAL_QUADROS * sizeof(struct TabelaPaginas));
    if (lista_tabelas.tabelas == NULL) {
        fprintf(stderr, "> Erro ao alocar memória para as tabelas de páginas\n");
        exit(1);
    }
    lista_tabelas.count = 0;

}

// Função para liberar a memória alocada para a lista de tabelas de páginas
void liberar_lista_tabelas() {
    free(lista_tabelas.tabelas);
}

// Função principal (menu interativo)
int main() {
    inicializar_quadros_livres();
    inicializar_lista_tabelas(); 

    int opcao = -1;
    int num_novo_processo;
    int tamanho_processo;
    int input_success;

    do {
        printf("\nMenu:\n");
        printf("1. Visualizar memoria fisica\n");
        printf("2. Criar processo (tamanho maximo %d bytes)\n", TAM_MAX_PROCESSO);
        printf("3. Visualizar tabela de paginas\n");
        printf("4. Visualizar processos\n");
        printf("5. Remover processo da memoria fisica\n");
        printf("6. Sair\n");
        printf("Escolha uma opcao: ");

        // Leitura segura da opção do menu
        input_success = scanf("%d", &opcao);

        // Verifica se a entrada é um número inteiro
        if (input_success != 1) {
            printf("\n> Opcao invalida. Escolha novamente.\n");

            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1:
                visualizar_memoria();
                break;
            case 2:
                printf("Digite o numero do processo entre 1 e 255: ");
                scanf("%d", &num_novo_processo);
				if (num_novo_processo > 0 && num_novo_processo < 256) {
					printf("Digite o tamanho do processo (tamanho maximo %d bytes / %d paginas):", TAM_MAX_PROCESSO, (TAM_MAX_PROCESSO/TAM_PAGINA));
					scanf("%d", &tamanho_processo);
					criar_processo(num_novo_processo, tamanho_processo);
				} else {
					printf("> Opcao invalida.\n");
				}
                break;
            case 3:
                printf("Digite o número do processo: ");
                scanf("%d", &num_novo_processo);
                visualizar_tabela_paginas(num_novo_processo);
                break;
            case 4:
                visualizar_processos();
                break;
            case 5:
				printf("Digite o numero do processo a ser removido: ");
                scanf("%d", &num_novo_processo);
				liberar_memoria_fisica(num_novo_processo);
                break;
			case 6:
                printf("Encerrando o simulador...\n");
            default:
                printf("> Opcao invalida. \n");
        }
    } while (opcao != 6);
    liberar_lista_tabelas(); // Liberar memória alocada para as tabelas de páginas
    return 0;
}
