import sys 
#Importando o pyplot
from matplotlib import pyplot as plt

nome = sys.argv[1] 
arq = open(nome, "r")

#lê o nome do arquivo
instancia=arq.readline()
#lê a melhor solução conhecida
melhor=arq.readline()

it = []
valor = []
otimo=[]
#lendo os dados das iterações
for linha in arq:
    valores = linha.split()
    it.append(int(valores[0]))
    valor.append(float(valores[1]))
    otimo.append(float(melhor))
    print('iteracao ', valores[0], 'valor ', valores[1] )


arq.close()

# Eixo_x, Eixo_y
plt.plot(it, valor,label='Busca Local')
plt.plot(it, otimo,label='Melhor Valor')
plt.xlabel('Iteração')
plt.ylabel('Valor')
plt.title(instancia)
plt.legend()
plt.grid()
#plt.xticks(range(min(it), max(it)+1))

#plt.autoscale(enable=True,axis='x',tight=True)
#plt.gca().set_aspect('equal', adjustable='box')
#plt.draw()
plt.show()