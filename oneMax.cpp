#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

random_device rd;
mt19937 gen(rd());

#define tamanhoIndividuo 20
#define tamanhoPopulacao 2 * tamanhoIndividuo
#define tamanhoTorneio tamanhoPopulacao / 4
#define probabilidadeCruzamento 70
#define probabilidadeMutacao 30

struct Individuo
{
  vector<int> genotipo = vector<int>(tamanhoIndividuo, 0);
  unsigned int fitness = 0;
};

unsigned int gerarNumAleatorio(int limite)
{
  uniform_int_distribution<> dis(0, limite);
  return dis(gen);
}

bool ordenar(Individuo a, Individuo b)
{
  return a.fitness > b.fitness;
}

void calcularFitness(vector<Individuo> &populacao)
{
  // Avaliando o fitness de cada individuo
  unsigned int fitnessValue = 0;
  // Avaliando o fitness de geral
  for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
  {
    for (int j = 0; j < tamanhoIndividuo; j++)
    {
      fitnessValue += populacao[individuo].genotipo[j];
    }
    populacao[individuo].fitness = fitnessValue;
    fitnessValue = 0;
  }
}

/**
 *
 * Calculo da soma das aptidoes: Some todas as aptidoes dos individuos na populacao, obtendo um valor total de aptidao da populacao.
 *
 * Calculo das proporcoes de selecao: Calcule a proporcao de selecao para cada individuo, dividindo a sua aptidao pela soma total das aptidoes da populacao. Essa proporcao representa a probabilidade de um individuo ser selecionado.
 *
 * Criacao da roleta: Construa uma roleta imaginaria em que cada individuo tenha uma fatia proporcional ao seu valor de proporcao de selecao. Isso pode ser feito usando uma matriz ou uma estrutura de dados adequada.
 *
 * Geracao de um numero aleatorio: Gere um numero aleatorio entre 0 e 1 para determinar qual fatia da roleta sera selecionada.
 *
 * Selecao do individuo: Percorra a roleta e encontre o intervalo onde o numero aleatorio se encaixa. O individuo correspondente a esse intervalo eh selecionado.
 *
 * Repeticao: Repita os passos 5 e 6 ateh obter o numero desejado de individuos selecionados.
 *
 * @param fitness Vector com os valores de fitness de cada individuo
 *
 * @return Indice do individuo selecionado
 */
unsigned int roleta(vector<Individuo> populacao)
{

  int fitnessTotal = 0;
  // Calculo da soma das aptidoes
  for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
  {
    fitnessTotal += populacao[individuo].fitness;
  }

  vector<int> probabilidadeIndividuos;

  // Calculo das proporcoes de selecao
  for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
  {
    const int probabilidade = 100 * populacao[individuo].fitness / fitnessTotal;
    probabilidadeIndividuos.push_back(probabilidade);
  }

  // Geracao de um numero aleatorio
  int sorteado = gerarNumAleatorio(99);
  int individuo = -1;
  int probAnterior = 0;

  // Selecao do individuo
  for (int i = 0; i < tamanhoPopulacao; i++)
  {
    if (probAnterior + probabilidadeIndividuos[i] >= sorteado)
    {
      individuo = i;
      break;
    }
    probAnterior += probabilidadeIndividuos[i];
  }

  if (individuo == -1)
    individuo = tamanhoPopulacao - 1;

  return individuo;
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
unsigned int torneio(vector<Individuo> populacao)
{
  // Salva a posicao do competidor na populacao
  vector<int> competidores;

  // Selecao dos concorrentes
  for (int i = 0; i < tamanhoTorneio; i++)
  {
    const unsigned int sorteado = gerarNumAleatorio(tamanhoPopulacao - 1);
    competidores.push_back(sorteado);
  }

  int maiorFitness = 0;
  int vencedor = 0;

  // Competicao
  for (int individuo = 0; individuo < tamanhoTorneio; individuo++)
  {
    if (populacao[competidores[individuo]].fitness > maiorFitness)
    {
      maiorFitness = populacao[competidores[individuo]].fitness;
      vencedor = competidores[individuo];
    }
  }
  return vencedor;
}

void cruzamento(Individuo &pai, Individuo &mae)
{
  if (gerarNumAleatorio(99) >= probabilidadeCruzamento)
    return; // Nao haverah cruzamento

  const unsigned int separador = tamanhoIndividuo / 2;

  for (int i = 0; i < tamanhoIndividuo; i++)
  {
    if (i < separador)
    {
      if (pai.genotipo[i] != mae.genotipo[i])
      {
        if (pai.genotipo[i] == 1)
          pai.fitness--;
        else
          pai.fitness++;
      }
      pai.genotipo[i] = mae.genotipo[i];
    }
    else
    {
      if (pai.genotipo[i] != mae.genotipo[i])
      {
        if (mae.genotipo[i] == 1)
          mae.fitness--;
        else
          mae.fitness++;
      }
      mae.genotipo[i] = pai.genotipo[i];
    }
  }
}

void mutacao(Individuo &individuo)
{
  if (gerarNumAleatorio(99) >= probabilidadeMutacao)
    return; // Nao haverah mutacao

  for (int i = 0; i < tamanhoIndividuo; i++)
  {
    if (gerarNumAleatorio(99) >= 50)
    {
      if (individuo.genotipo[i] == 0)
      {
        individuo.genotipo[i] = 1;
        individuo.fitness++;
      }
      else
      {
        individuo.genotipo[i] = 0;
        individuo.fitness--;
      }
    }
  }
}

void imprimirFitnessPopulacao(vector<Individuo> populacao)
{
  unsigned int soma = 0;
  for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
  {
    soma += populacao[individuo].fitness;
  }
  cout << "Media da populacao: " << soma / (tamanhoPopulacao) << endl;
}

void imprimiIndividuo(Individuo individuo)
{
  cout << "Individuo: ";
  for (int i = 0; i < tamanhoIndividuo; i++)
  {
    cout << individuo.genotipo[i] << "|";
  }
  cout << endl;
}

int main()
{

  // Inicializacao da populacao
  // Populacao = Vetor do individuo, sua fitness
  vector<Individuo> populacaoInicial(tamanhoPopulacao, {vector<int>(tamanhoIndividuo, 0), 0});
  for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
  {
    for (int j = 0; j < tamanhoIndividuo; j++)
    {
      const unsigned int randomNumber = gerarNumAleatorio(99);
      populacaoInicial[individuo].genotipo[j] = randomNumber >= 50 ? 1 : 0;
    }
  }

  calcularFitness(populacaoInicial);
  cout << "Inicial: \n";
  imprimirFitnessPopulacao(populacaoInicial);

  Individuo melhorIndividuo;
  bool continuar = true;
  unsigned int geracoes = 0;
  while (continuar)
  {
    vector<Individuo> sorteados;
    for (int i = 0; i < tamanhoPopulacao; i++)
    {
      const unsigned int individuo = roleta(populacaoInicial);
      // const unsigned int individuo = torneio(populacaoInicial);
      sorteados.push_back(populacaoInicial[individuo]);
    }

    for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
    {
      const unsigned int conjuge = roleta(sorteados);
      // const unsigned int conjuge = torneio(sorteados);
      cruzamento(sorteados[individuo], sorteados[conjuge]);
    }

    // Selecao e mutacao
    for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
    {
      mutacao(sorteados[individuo]);
    }

    // Eletismo, procurar o melhor
    unsigned int maior = 0;
    unsigned int indiceMaior = 0;
    unsigned int menor = INT8_MAX;
    unsigned int indiceMenor = 0;
    Individuo melhorIndividuoAtual;
    for (int individuo = 0; individuo < tamanhoPopulacao; individuo++)
    {
      if (sorteados[individuo].fitness > maior)
      {
        maior = sorteados[individuo].fitness;
        indiceMaior = individuo;
      }
      if (sorteados[individuo].fitness < menor)
      {
        indiceMenor = individuo;
        menor = sorteados[individuo].fitness;
      }
    }
    melhorIndividuoAtual = sorteados[indiceMaior];

    if (melhorIndividuoAtual.fitness == tamanhoIndividuo)
      continuar = false;

    if (melhorIndividuoAtual.fitness > melhorIndividuo.fitness)
      melhorIndividuo = melhorIndividuoAtual;
    else
      sorteados[indiceMenor] = melhorIndividuo;

    populacaoInicial = sorteados;
    geracoes++;
  }

  cout << "\nFinal: \n";
  imprimirFitnessPopulacao(populacaoInicial);
  cout << "Geracoes: " << geracoes;
}