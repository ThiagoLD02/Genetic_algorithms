#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <climits> // INT_MAX

using namespace std;
random_device rd;
mt19937 gen(rd());

#define tamanhoPopulacao 40
#define probabilidadeCruzamento 70
#define probabilidadeMutacao 30

typedef unsigned int UI;

struct voo {
    string origem;
    string destino;
    UI horarioIda;
    UI horarioChegada;
    UI preco;
};

// Um individuo eh um grupo de 6 voos
struct Individuo {
    vector<voo> voos;
    UI fitness = 0;
};

UI gerarNumAleatorio(int limite) {
    uniform_int_distribution<> dis(0, limite);
    return dis(gen);
}

void calcularFitness(Individuo &individuo) {

    // Avaliando o tempo de espera
    UI menor = INT_MAX;
    UI maior = 0;
    UI totalGasto = 0;
    for (UI i = 0; i < 6; i++) {
        totalGasto += individuo.voos[i].preco;
        if (individuo.voos[i].horarioChegada < menor)
            menor = individuo.voos[i].horarioChegada;
        if (individuo.voos[i].horarioChegada > maior)
            maior = individuo.voos[i].horarioChegada;
    }
    const UI tempoDeEspera = maior - menor;

    individuo.fitness = (tempoDeEspera + totalGasto);
}

/**
 *
 * Tamanho do torneio: Defina um tamanho para o torneio, que eh o numero de individuos que competirao em cada torneio. Geralmente, esse valor eh pequeno em relacao ao tamanho total da populacao.
 *
 * Selecao dos concorrentes: Selecione aleatoriamente um numero de individuos igual ao tamanho do torneio da populacao atual. Esses individuos serao os competidores no torneio.
 *
 * Competicao: Compare a aptidao dos competidores selecionados. O individuo com a maior aptidao eh selecionado como o vencedor do torneio. Em caso de empate, pode-se escolher aleatoriamente um dos competidores com a maior aptidao.
 *
 * Repeticao: Repita os passos 2 e 3 ateh obter o numero desejado de vencedores do torneio, que serao os individuos selecionados para reproducao.
 *
 * @param fitness Vector com os valores de fitness de cada individuo
 *
 * @return Indice do individuo selecionado
 */
UI torneio(vector<Individuo> populacao) {
    // Salva a posicao do competidor na populacao
    vector<UI> competidores;
    UI tamanhoTorneio = gerarNumAleatorio(tamanhoPopulacao - 1);
    // Selecao dos concorrentes
    for (int i = 0; i < tamanhoTorneio; i++) {
        const UI sorteado = gerarNumAleatorio(tamanhoPopulacao - 1);
        competidores.push_back(sorteado);
    }

    UI menorFitness = INT_MAX;
    UI vencedor = 0;

    // Competicao
    for (UI individuo = 0; individuo < tamanhoTorneio; individuo++) {
        if (populacao[competidores[individuo]].fitness < menorFitness) {
            menorFitness = populacao[competidores[individuo]].fitness;
            vencedor = competidores[individuo];
        }
    }
    return vencedor;
}

void cruzamento(Individuo &pai, Individuo &mae) {
    if (gerarNumAleatorio(99) >= probabilidadeCruzamento)
        return; // Nao haverah cruzamento

    // Sortear pontos de cruzamento
    // modificar, n pode ser 0 nem o limite da quantidade de individuos
    UI pontoFinal = 0;
    while (pontoFinal == 0)
        pontoFinal = gerarNumAleatorio(5);

    for (UI i = 0; i < pontoFinal; i++) {
        voo aux = pai.voos[i];
        pai.voos[i] = mae.voos[i];
        mae.voos[i] = aux;
    }
}

bool diferentes(const voo &a, const voo &b) {
    return !(a.destino == b.destino &&
             a.horarioChegada == b.horarioChegada &&
             a.horarioIda == b.horarioIda &&
             a.origem == b.origem &&
             a.preco == b.preco);
}

void mutacao(Individuo &individuo, map<UI, vector<voo>> dados) {
    if (gerarNumAleatorio(99) >= probabilidadeMutacao)
        return; // Nao haverah mutacao


    for (int i = 0; i < 2; ++i) {

        UI cidade = gerarNumAleatorio(5);
        // Sortear um horario disponivel a partir desta cidade de origem
        const int qtdVoosDisponiveis = dados.at(cidade).size() - 1;
        UI indiceHorarioSorteado = gerarNumAleatorio(qtdVoosDisponiveis);

        voo vooSorteado = dados.at(cidade).at(indiceHorarioSorteado);
        if (diferentes(vooSorteado, individuo.voos[cidade]))
            individuo.voos[cidade] = vooSorteado;
        else {
            UI outroIndiceHorarioSorteado = indiceHorarioSorteado;
            while (outroIndiceHorarioSorteado == indiceHorarioSorteado) {
                outroIndiceHorarioSorteado = gerarNumAleatorio(qtdVoosDisponiveis);
            }
            individuo.voos[cidade] = dados[cidade].at(qtdVoosDisponiveis);
        }
    }


}

void imprimirFitnessPopulacao(vector<Individuo> populacao) {
    unsigned int soma = 0;
    for (int individuo = 0; individuo < tamanhoPopulacao; individuo++) {
        soma += populacao[individuo].fitness;
    }
    cout << "Media da populacao: " << soma / (tamanhoPopulacao) << endl << endl;
}

void imprimiIndividuo(Individuo individuo) {
    cout << "Individuo:\n";
    for (int i = 0; i < 6; ++i) {
        cout << individuo.voos[i].origem << ", "
             << individuo.voos[i].destino << ", "
             << individuo.voos[i].horarioIda << ", "
             << individuo.voos[i].horarioChegada << ", "
             << individuo.voos[i].preco << "\n";
    }
    cout << "Fitness: " << individuo.fitness << endl << endl;
}

map<UI, vector<voo>> lerTxt(vector<vector<string>> &substringsLinhas) {
    ifstream arquivo("flights.txt");
    if (!arquivo) {
        throw runtime_error("Erro ao abrir o arquivo.");
    }
    string linha;
    while (getline(arquivo, linha)) {
        vector<string> substrings;
        stringstream ss(linha);
        string substring;

        while (getline(ss, substring, ',')) {
            substrings.push_back(substring);
        }

        substringsLinhas.push_back(substrings);
    }
    arquivo.close();
    vector<voo> dadosPessoas;
    for (const vector<string> &substrings: substringsLinhas) {

        // Verificar se ha substrings suficientes para criar uma instancia de voo
        if (substrings.size() >= 5) {
            voo dados;

            // Atribuir os valores das substrings à struct voo
            dados.origem = substrings[0];
            dados.destino = substrings[1];

            // Converter as substrings numericas para valores inteiros
            auto indice = substrings[2].find(':');
            if (indice != string::npos) {
                const int hora = stoi(substrings[2].substr(0, indice));
                const int minuto = stoi(substrings[2].substr(indice + 1));
                dados.horarioIda = hora * 60 + minuto;
            }
            indice = substrings[3].find(':');
            if (indice != string::npos) {
                const int hora = stoi(substrings[3].substr(0, indice));
                const int minuto = stoi(substrings[3].substr(indice + 1));
                dados.horarioChegada = hora * 60 + minuto;
            }
            dados.preco = stoi(substrings[4]);

            // Adicionar a struct voo ao vetor dadosPessoas
            dadosPessoas.push_back(dados);
        }
    }

    map<UI, vector<voo>> mapa;
    for (const auto &dados: dadosPessoas) {
        const string origem = dados.origem;
        UI cidade;
        if (origem == "LIS")
            cidade = 0;
        else if (origem == "MAD")
            cidade = 1;
        else if (origem == "CDG")
            cidade = 2;
        else if (origem == "DUB")
            cidade = 3;
        else if (origem == "BRU")
            cidade = 4;
        else if (origem == "LHR")
            cidade = 5;
        else if (origem == "FCO")
            cidade = 6;
        mapa[cidade].push_back(dados);
    }

    return mapa;
}

int main() {
    // Ler e armazenar todas as opcoes
    vector<vector<string>> substringsLinhas; // Vetor para armazenar as substrings de cada linha do txt
    const map<UI, vector<voo>> listaDeVoos = lerTxt(substringsLinhas);

    // Inicializacao da populacao
    // Populacao == Vetor do individuo, sua fitness
    vector<Individuo> populacaoInicial(tamanhoPopulacao, {vector<voo>(6, {"", "", 0, 0, 0}), 0});

    for (UI individuo = 0; individuo < tamanhoPopulacao; individuo++) {
        for (UI cidade = 0; cidade < 6; cidade++) {
            const int qtdHorarios = listaDeVoos.at(cidade).size();
            const UI numeroAleatorio = gerarNumAleatorio(qtdHorarios - 1);
            populacaoInicial[individuo].voos[cidade] = listaDeVoos.at(cidade).at(numeroAleatorio);
        }
    }

    cout << "Inicial: \n";
    for (UI i = 0; i < tamanhoPopulacao; i++) {
        calcularFitness(populacaoInicial[i]);
//        imprimiIndividuo(populacaoInicial[i]);
    }

    imprimirFitnessPopulacao(populacaoInicial);


    Individuo melhorIndividuo;
    melhorIndividuo.fitness = INT_MAX;
    bool continuar = true;
    unsigned int geracoes = 0;
    while (continuar) {
        // Sorteados para talvez cruzar e/ou mutar
        vector<Individuo> sorteados;
        for (UI i = 0; i < tamanhoPopulacao; i++) {
            const UI individuo = torneio(populacaoInicial);
            // const unsigned int individuo = torneio(populacaoInicial);
            sorteados.push_back(populacaoInicial[individuo]);
        }

        for (UI individuo = 0; individuo < tamanhoPopulacao; individuo++) {
            const UI conjuge = torneio(sorteados);
            cruzamento(sorteados[individuo], sorteados[conjuge]);
        }

        // Mutacao
        for (UI individuo = 0; individuo < tamanhoPopulacao; individuo++) {
            mutacao(sorteados[individuo], listaDeVoos);
        }

        for (UI i = 0; i < tamanhoPopulacao; i++) {
            calcularFitness(sorteados[i]);
        }

        // Eletismo, procurar o melhor
        UI maiorFitness = 0;
        UI indiceMaiorFitness = 0;
        UI menorFitness = INT_MAX;
        UI indiceMenorFitness = 0;
        Individuo melhorIndividuoAtual;
        for (int individuo = 0; individuo < tamanhoPopulacao; individuo++) {
            if (sorteados[individuo].fitness > maiorFitness) {
                maiorFitness = sorteados[individuo].fitness;
                indiceMaiorFitness = individuo;
            }
            if (sorteados[individuo].fitness < menorFitness) {
                indiceMenorFitness = individuo;
                menorFitness = sorteados[individuo].fitness;
            }
        }
        melhorIndividuoAtual = sorteados[indiceMenorFitness];

        if (melhorIndividuoAtual.fitness < melhorIndividuo.fitness)
            melhorIndividuo = melhorIndividuoAtual;
        else
            sorteados[indiceMaiorFitness] = melhorIndividuo;

        populacaoInicial = sorteados;

        geracoes++;
        continuar = geracoes != 200;
    }

    cout << "\nFinal: \n";
    imprimirFitnessPopulacao(populacaoInicial);
    cout << "Geracoes: " << geracoes << endl;
    cout << "\nMelhor individuo: \n";
    imprimiIndividuo(melhorIndividuo);
}

/*

    Salvar todos os melhores individuos de cada geração
    rodar 30 vezes
    desvio padrão, variáncia


    Diferença entre o que sai mais cedo e oq sai mais tarde


*/