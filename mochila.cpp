#include<fstream>
#include<iostream>
#include <dirent.h> //ler diretorio de arquivos
#include <ctime>
#include <iomanip> 
#include <stdlib.h>
#include <string.h>

using namespace std;

//estrutura para armazenar as informacoes dos itens 
struct tipoItem
{
    float peso; //peso do item
    float valor; //valor do item
};

//estrutura para armazenar solucoes da mochila
struct tipoSolucao
{
    int n; //Quantidade  itens
    float peso; //peso total da solucao
    float valor; //valor total da solucao
    int *item; //mochila - tem um vetor binario para representar os itens
    float capacidade; //capacidade maxima
    float otimo; //solucao otima encontrada
};

//zera as variaveis da solucao e aloca de forma dinamica o vetor binario de solucao
void criaSolucao(tipoSolucao &s,int n,float capacidade, float otimo)
{
	/* crio uma solucao de uma mochila vazia */
    s.n=n;
    s.peso=0;
    s.valor=0;
    s.capacidade=capacidade;
    s.otimo=otimo;
    s.item= new int[n];
    for(int i=0;i<n;i++)
        s.item[i]=0;
}

//apaga o vetor binário de solucao que foi criado dinamicamente.
void apagaSolucao(tipoSolucao &s)
{
    delete s.item;
}

//funcao Le o diretorio com as instancia e abre um menu de selecao e retorna a string contendo o nome do arquivo da instancia
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

//Le do arquivo os dados das instancia 
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

//imprime os dados de uma solucao
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

//cria uma solucao aleatoria
void solucaoInicial(tipoSolucao &s,tipoItem itens[])
{
    int *marca=new int[s.n]; //marcar quais jÃ¡ foram testados
    int i,cont=0;
    for(i=0;i<s.n;i++)
    {
        marca[i]=0;
    }

    while(s.peso<s.capacidade && cont<s.n) //enquanto o peso for menor que a  capacidade e o cont menor que o minimo
    {
        i=rand()%s.n; //sorteio uma posicao aleatoria
        while(marca[i]==1) 
            i=(i+1)%s.n;  	//se cair numa solucao ocupada, eu ando pra frente, de forma circular ou seja chego no ultimo e volto no primeiro
                   
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

//avalia a vizinhanca de uma solucao e retorna true se encontrou um vizinho melhor.
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
    
 /*-------------------Usado para criar o arquivo de saÃ­da--------------------------*/   
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

    solucaoInicial(s,itens); //se eu comento aqui a solucao inicial Ã© zero
    imprimeSolucao(s,itens);

    fout<<i<<" "<<s.valor<<"\n"; //gravando a solucao inicial

   /*
        Para o VND eu faÃ§Ãµ a busca, se nÃ£o achei eu amplio minha vizinhanÃ§a
        para o VNS Ã© necessario realizar a agitaÃ§Ã£o
   */
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


void VND(tipoSolucao &s, tipoItem itens[], char nome[])
{
    
    bool teste=true;
    int d=2;	//distancia de busca
    int i=0;
    
 /*-------------------Usado para criar o arquivo de saÃ­da--------------------------*/   
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

    solucaoInicial(s,itens); //se eu comento aqui a soluÃ§Ã£o inicial Ã© zero
    imprimeSolucao(s,itens);

    fout<<i<<" "<<s.valor<<"\n"; //gravando a soluÃ§Ã£o inicial

   /*
        Para o VND eu faÃ§Ãµ a busca, se nÃ£o achei eu amplio minha vizinhanÃ§a
        para o VNS Ã© necessario realizar a agitaÃ§Ã£o
   */
    while(teste && d <= 2)
    {
        teste=avaliaVizinhanca(s,itens,d);
        if(teste)
        {
            i++;
            imprimeSolucao(s,itens);
            fout<<i<<" "<<s.valor<<"\n"; //gravando a soluÃ§Ã£o atual
            d = 1;
        }else
        {
            cout<<"Ampliando o espaco de busca"<<endl;
            d++;
        }
        
    }
    fout.close(); //fechar o arquivo de saÃ­da
}

void sorteio(tipoSolucao s,tipoSolucao &ss, tipoItem itens[], int d)
{   
    int *marca=new int[s.n]; //marcar quais jÃ¡ foram testados
    int count = 1, i, peso_aux = 0,valor_aux = 0;
    ss = s;
    cout<<"itens antes do sorteio"<<endl;
    cout<<"Valor de d: "<<d<<endl;
    cout<<" mochila antes do sorteio"<<endl;
    for(i=0;i<s.n;i++)
    {
        marca[i]=0;
        cout<<s.item[i];
    }

    cout<<endl;
    while(count <= d)//Enquanto eu não troquei os bits necessesario eu permanesso no while
    {
        i=rand()%s.n; //sorteio uma posicao aleatoria
        while(marca[i]==1) 
            i=(i+1)%ss.n;  	//se cair numa solucao ocupada, eu ando pra frente, de forma circular ou seja chego no ultimo e volto no primeiro

        if(ss.item[i]==0)
        {
            peso_aux=ss.peso+itens[i].peso;
            valor_aux=ss.valor+itens[i].valor;         
        }
        else
        {
            peso_aux = ss.peso-itens[i].peso;
            valor_aux = ss.valor-itens[i].valor;
        }
        if(peso_aux < ss.capacidade)
        {

            ss.item[i]=(1+ss.item[i])%2;
            ss.peso = peso_aux;
            ss.valor = valor_aux;
            count++;
        }else
        {
            peso_aux = 0;
            valor_aux = 0;

        }
        marca[i]=1;
    }
    //itens depois o sorteio
    cout<<endl<<" mochila apos do sorteio"<<endl;
    for(i=0;i<ss.n;i++)
    {
        cout<<ss.item[i];
    }
    imprimeSolucao(ss,itens);
    delete marca;
    
}


void VNS(tipoSolucao &s, tipoItem itens[], char nome[])
{
	
/*------------------- variaveis Locais -------------------------------------------*/
	bool teste=true;
    int d = 1,numItera = 0, numItera_max = 5;
    int i=0;
    tipoSolucao ss;
    

/*-------------------Usado para criar o arquivo de saida--------------------------*/   
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
    
	cout<< "X recebe a solucao inicial"<<endl;
    solucaoInicial(s,itens); //se eu comento aqui a solucao inicial Ã© zero
    imprimeSolucao(s,itens);
    fout<<i<<" "<<s.valor<<"\n"; //gravando a solucao inicial

    while((numItera <= numItera_max) && (ss.valor != ss.otimo))
    {
		sorteio(s, ss, itens, d);
		teste = true;
	    
		 while(teste && d <= 2)
	    {
	        teste=avaliaVizinhanca(ss,itens,d);
	        if(teste)
	        {
	        	
				
	            i++;
	            imprimeSolucao(ss,itens);
	             s = ss;
	            fout<<i<<" "<<s.valor<<"\n"; //gravando a solucao atual
	            d = 1;
		   		
	        }else
	        {
	            cout<<"Ampliando o espaco de busca"<<endl;
	            d++;
	        }
	        
	    }
	    numItera++;
	}
    fout.close(); //fechar o arquivo de saida


}
int main()
{
    tipoItem *itens; //crio um vetor de itens
    float otimo; //variavel para armazenar o otimo
    float capacidade; // variavel para armazenar a capacidade
    int n; // variavÃ©l para armezanar quantidade de itens
    tipoSolucao sOtima; // crio uma variavel tipo solucao
    char nome[100];
    
    //criando a semente dos nÃºmeros aleatÃ³rio


    unsigned seed = time(NULL); // Retorna o valor em segundos desde janeiro de 1970
    srand(seed); //crio um nÃºmero aleatorio de acordo com a seed

    //escolhe e carrega os dados das instancia
    carregaDados(&itens,capacidade,n,otimo,nome);
    //imprimeItens(itens,n);
    
    //cria a solucao (zera e aloca o vetor binario)
    criaSolucao(sOtima,n,capacidade,otimo);
    
    //funcoes de otimizacao
    //buscaLocal(sOtima, itens,nome);
    VNS(sOtima, itens,nome);
    
    //melhor solucao
    cout<<"\n\n Melhor Solucao:";
    imprimeSolucao(sOtima,itens);

    cout<<"Arquivo de saida criado: "<<nome<<"_saida.txt";
    
    //apaga o vetor binario criado de forma dinamica
    apagaSolucao(sOtima);
    
    //apaga o vetor de itens;
    delete itens;
    return 0;
}

