#include<fstream>
#include<iostream>
#include <dirent.h> //ler diretorio de arquivos
#include <ctime>
#include <iomanip> 
#include <stdlib.h>
#include <string.h>

using namespace std;

//estrutura para armazenar as informações dos itens 
struct tipoItem
{
    float peso; //peso do item
    float valor; //valor do item
};

//estrutura para armazenar soluções da mochila
struct tipoSolucao
{
    int n; //Quantidade  itens
    float peso; //peso total da solução
    float valor; //valor total da solução
    int *item; //mochila - tem um vetor binario para representar os itens
    float capacidade; //capacidade máxima
    float otimo; //solução ótima encontrada
};

//zera as variavéis da solução e aloca de forma dinâmica o vetor binário de solução
void criaSolucao(tipoSolucao &s,int n,float capacidade, float otimo)
{
	/* crio uma solução de uma mochila vazia */
    s.n=n;
    s.peso=0;
    s.valor=0;
    s.capacidade=capacidade;
    s.otimo=otimo;
    s.item= new int[n];
    for(int i=0;i<n;i++)
        s.item[i]=0;
}

//apaga o vetor binário de solução que foi criado dinamicamente.
void apagaSolucao(tipoSolucao &s)
{
    delete s.item;
}

//função Lê o diretório com as instâncias e abre um menu de seleção e retorna a string contendo o nome do arquivo da instÃ¢ncia
//e do nome do arquivo com a resposta. 
void selecionaInstancia(char nomeArq1[], char nomeArq2[], char nome[])
{
    DIR *d;
    int op;
    struct dirent *dir;
    
    cout<<"\nTipo de instancia\n================\n[1] Large Scale\n[2] Low Dimensional\n\nDigite uma opcao: ";
    cin>>op;

    switch(op)
    {
        case 1: 
            strcpy(nomeArq1,"instances_01_KP\\large_scale");
            strcpy(nomeArq2,"instances_01_KP\\large_scale-optimum");  
            break;
        case 2:
            strcpy(nomeArq1,"instances_01_KP\\low-dimensional");
            strcpy(nomeArq2,"instances_01_KP\\low-dimensional-optimum"); 
            break;
        default:
            cout<<"Opcao invalida"; 
            exit(0);            
    }

    d = opendir(nomeArq1); //abre o diretorio escolhido
    if (d)
    {
        cout<<"\nInstancias\n=========\n";
        int c=0;
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
            {
                c++;
                cout<<"["<<setw(2)<<c<<"] "<<dir->d_name<<endl;
            }
        }
        cout<<"\nDigite uma opcao: ";
        cin>>op;
        if(op<=0 || op >c)
        {
            cout<<"Opcao invalida"; 
            closedir(d);
            exit(0);
        }
        rewinddir(d);

        c=0;
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
            {
                c++;
                if(c==op)
                {
                    strcat(nomeArq1,"\\"); //strcat concatena duas strings
                    strcat(nomeArq1,dir->d_name);
                    strcat(nomeArq2,"\\");
                    strcat(nomeArq2,dir->d_name);
                    strcpy(nome,dir->d_name);
                    break;
                }
            }
        }

        closedir(d);
    }
}

//Lê do arquivo os dados das instâncias 
 void carregaDados(tipoItem **itens, float &capacidade,int &n,float &otimo, char nome[])
{
    char nomeArq1[300],nomeArq2[300];
    ifstream fin;
    
    selecionaInstancia(nomeArq1, nomeArq2,nome);

    cout<<"\nArquivo da instancia escolhida: "<<nomeArq1<<endl;
    cout<<"\nArquivo contendo a solucao otima da instancia: "<<nomeArq2<<endl;

    fin.open(nomeArq2);
    if(!fin.is_open())
    {
        cout<<"Erro ao abrir o arquivo \""<<nomeArq2<<"\""<<endl;
        exit(0);
    }
    fin>>otimo;
    cout<<"\nValor Otimo: "<<otimo<<endl;      
    fin.close();

    fin.open(nomeArq1);
    if(!fin.is_open())
    {
        cout<<"Erro ao abrir o arquivo \""<<nomeArq1<<"\""<<endl;
        exit(0);
    }

    fin>>n>>capacidade;
    //cout<<"numero de itens: "<<n<<" capacidade: "<<capacidade<<endl;
    *itens = new tipoItem[n];
    for(int i=0;i<n;i++)
    {
        fin>>(*itens)[i].valor>>(*itens)[i].peso;
       //cout<<"item "<<i+1<<" -- valor: "<<(*itens)[i].valor<<" peso: "<<(*itens)[i].peso<< endl;
    }
    fin.close();
 
}

//imprime o vetor com os itens
void imprimeItens(tipoItem itens[], int n)
{
    for(int i=0;i<n;i++)
    {
        cout<<"Item "<<setw(4)<<i+1;
        cout<<"   |  Peso: "<<setw(5) <<itens[i].peso;
        cout<<"   |  Valor: "<<setw(5) <<itens[i].valor<<endl;
    }
    cout<<endl;
}

//imprime os dados de uma solução
void imprimeSolucao( tipoSolucao s,tipoItem itens[] )
{
    cout<<" Solucao\n";
    cout<<"=======================================\n";
    cout<<"Valor Otimo:"<<s.otimo<<endl;
    cout<<"Capacidade Maxima: "<<s.capacidade<<endl;
    cout<<"=======================================\n";
    cout<<"Valor Atual: "<<s.valor<<endl;
    cout<<"Peso Atual: "<<s.peso<<endl<<endl;

   // cout<<"Escolhidos:\n";
    int cont=0;
    for(int i=0;i<s.n;i++)
    {
        if(s.item[i]==1)
        {
            cont++;
            //cout<<"Item "<<setw(4)<<i+1;
            //cout<<"   |  Peso: "<<setw(5) <<itens[i].peso;
            //cout<<"   |  Valor: "<<setw(5) <<itens[i].valor<<endl;
        }
    }
    
    cout<<"Itens escolhidos: "<<cont<<endl<<endl;    
    cout<<endl;
}

//cria uma solução aleatória
void solucaoInicial(tipoSolucao &s,tipoItem itens[])
{
    int *marca=new int[s.n]; //marcar quais já foram testados
    int i,cont=0;
    for(i=0;i<s.n;i++)
    {
        marca[i]=0;
    }

    while(s.peso<s.capacidade && cont<s.n) //enquanto o peso for menor que a  capacidade e o cont menor qu eo máximo
    {
        i=rand()%s.n; //sorteio uma posição aleatoria
        while(marca[i]==1) 
            i=(i+1)%s.n;  	//se cair numa solução ocupada, eu ando pra frente, de forma circular ou seja chego no ultimo e volto no primeiro
                   
        if(s.peso+itens[i].peso<=s.capacidade)
        {
            s.item[i]=1;
            s.peso=s.peso+ itens[i].peso;
            s.valor=s.valor+itens[i].valor;
        }
        marca[i]=1;
        cont++;        
    }
        
    delete marca;
        
}

//avalia a vizinhança de uma solução e retorna true se encontrou um vizinho melhor.
bool avaliaVizinhanca(tipoSolucao &s, tipoItem itens[], int d)
{
    int i,j, pos1=-1, pos2=-1, posd1=-1;
    float mValor=s.valor, mPeso=s.peso;
    float peso_aux, valor_aux,peso_aux2, valor_aux2;
    bool melhorou=false;
    
    switch(d)
    {
        case 1:
            for(i=0;i<s.n;i++)
            {
               if(s.item[i]==0)
               {
                    peso_aux=s.peso+itens[i].peso;
                    valor_aux=s.valor+itens[i].valor;         
                }
                else
                {
                    peso_aux=s.peso-itens[i].peso;
                    valor_aux=s.valor-itens[i].valor;
                }
                if(peso_aux<=s.capacidade && valor_aux>mValor)
                //if(peso_aux<=s.capacidade && valor_aux-mValor>0.01)
                {
                    pos1=i;
                    mValor=valor_aux;
                    mPeso=peso_aux;
                }
            }
            if(pos1!=-1)
            {
                s.item[pos1]= (s.item[pos1]+1)%2;
                s.valor=mValor;
                s.peso=mPeso;
                return true;
            }
            return false;
        break;

       case 2:
           //d=1
            for(i=0;i<s.n;i++)
            {
               if(s.item[i]==0)
               {
                    peso_aux=s.peso+itens[i].peso;
                    valor_aux=s.valor+itens[i].valor;         
                }
                else
                {
                    peso_aux=s.peso-itens[i].peso;
                    valor_aux=s.valor-itens[i].valor;
                }
                if(peso_aux<=s.capacidade && valor_aux>mValor)
                {
                    posd1=i;
                    mValor=valor_aux;
                    mPeso=peso_aux;

                }
            }
            if(posd1!=-1)
            {
                melhorou=true;
            }

            //d=2
            for(i=0;i<s.n-1;i++)
            {
                if(s.item[i]==0)
                {
                    peso_aux=s.peso+itens[i].peso;
                    valor_aux=s.valor+itens[i].valor;         
                }
                else
                {
                    peso_aux=s.peso-itens[i].peso;
                    valor_aux=s.valor-itens[i].valor;
                }
                            
                for(j=i+1;j<s.n;j++)
                {
                
                    if(s.item[j]==0)
                    {   
                        peso_aux2=peso_aux+itens[j].peso;
                        valor_aux2=valor_aux+itens[j].valor;         
                    }
                    else
                    {
                        peso_aux2=peso_aux-itens[j].peso;
                        valor_aux2=valor_aux-itens[j].valor;
                    }

                    if(peso_aux2<=s.capacidade && valor_aux2>mValor)
                    {
                        pos1=i;
                        pos2=j;
                        mValor=valor_aux2;
                        mPeso=peso_aux2;
                    }
                }

            } 
            if(pos1!=-1)
            {
                s.item[pos1]=(s.item[pos1]+1)%2;
                s.item[pos2]=(s.item[pos2]+1)%2;
                s.valor=mValor;
                s.peso=mPeso;
                return true;
            }
            if(melhorou)
            {
                s.item[posd1]= (s.item[posd1]+1)%2;
                s.valor=mValor;
                s.peso=mPeso;
                return true;
            }
            return false;
        break;
  
    }
    return false;
}


void buscaLocal(tipoSolucao &s, tipoItem itens[], char nome[])
{
    
    bool teste=true;
    int d=2;	//distancia de busca
    int i=0;
    
 /*-------------------Usado para criar o arquivo de saída--------------------------*/   
    ofstream fout;
    char nomearq[100];

    
    strcpy(nomearq,nome);
    strcat(nomearq,"_saida.txt");
    fout.open(nomearq);
    if(!fout.is_open())
    {
        cout<<"Erro ao criar o arquivo saida.txt"<<endl;
        exit(0);
    }
    fout<<nome<<"\n";
    fout<<s.otimo<<"\n";
    /*--------------------------------------------------------------------------------*/

    solucaoInicial(s,itens); //se eu comento aqui a solução inicial é zero
    imprimeSolucao(s,itens);

    fout<<i<<" "<<s.valor<<"\n"; //gravando a solução inicial
   
    while(teste)
    {
        teste=avaliaVizinhanca(s,itens,d);
        if(teste)
        {
            i++;
            imprimeSolucao(s,itens);
            fout<<i<<" "<<s.valor<<"\n"; //gravando a soluÃ§Ã£o atual
        }
        
    }
    fout.close(); //fechar o arquivo de saÃ­da
}




int main()
{
    tipoItem *itens; //crio um vetor de itens
    float otimo; //variavél para armazenar o ótimo
    float capacidade; // variavél para armazenar a capacidade
    int n; // variavél para armezanar quantidade de itens
    tipoSolucao sOtima; // crio uma variavel tipo solução
    char nome[100];
    
    //criando a semente dos números aleatório
    unsigned seed = time(NULL); // Retorna o valor em segundos desde janeiro de 1970
    srand(seed); //crio um número aleatorio de acordo com a seed

    //escolhe e carrega os dados das instância
    carregaDados(&itens,capacidade,n,otimo,nome);
    //imprimeItens(itens,n);
    
    //cria a solução (zera e aloca o vetor binário)
    criaSolucao(sOtima,n,capacidade,otimo);
    
    //função de otimização
    buscaLocal(sOtima, itens,nome);
    
    //melhor solução
    cout<<"\n\n Melhor Solucao:";
    imprimeSolucao(sOtima,itens);

    cout<<"Arquivo de saida criado: "<<nome<<"_saida.txt";
    
    //apaga o vetor binário criado de forma dinâmica
    apagaSolucao(sOtima);
    
    //apaga o vetor de itens;
    delete itens;
    return 0;
}
