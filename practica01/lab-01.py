# Importa todas las funciones del módulo math y random
from math import *
from random import *
import random

# Función que genera el conjunto Zn, es decir, los números menores que 'n'
# que son coprimos con 'n' (tienen MCD igual a 1 con 'n')
def Zn(n):
    l = [1]
    for i in range(2, n):
        if gcd(i, n) == 1:
            l.append(i)
    return l

# Función para calcular el inverso modular de 'a' módulo 'n'
def modInver(n, a):
    l = Zn(n)
    print("")
    if a in l:
        for i in l:
            if(((i * a) % n) == 1):   # Si encuentra un número cuyo producto con 'a' da 1 módulo 'n'
                return i
    else:        
        return -1

# Función para generar dos claves aleatorias
def randomKeys(n):
    l = Zn(n)
    l = [x for x in l if x not in (0, 1)]          # Filtra los valores 0 y 1, ya que no son útiles en el contexto de claves 
    b = random.randint(2, n-1)                     # Selecciona un número aleatorio 'b' entre 2 y n-1
    a = random.choice([x for x in l if x != b])    # Selecciona un número 'a' de Zn(n) distinto de 'b'

    print(f"Tus llaves son: K({a}, {b})")


while(True):
    print("")
    print("[1] Generar valores de Zn*.")
    print("[2] Generar a^-1 mod n.")
    print("[3] Generar a & b key.")
    print("[0] Salir")
    opc = int(input("Selecciona una opción: "))
    print("")

    match(opc):
        case 1:     # Opción para generar Zn*
            n = int(input("Ingresa el valor de n: "))
            if n < 1:
                print("¡Ingresa un número mayor que 2!")

            else:
                lista = Zn(n)

            # Imprime el conjunto Zn* en formato de conjunto matemático
            print("Z" + str(n) + "* = {", end="")
            for i in lista:
                print(i, end="")
                if i != lista[-1]:
                    print(", ", end="")
            print("}")
            print("_____________________________")

            
        case 2:    # Opción para calcular el inverso modular
            n = int(input("Ingresa el valor de n: "))
            if n < 1:
                print("¡Ingresa un número mayor que 2!")

            else:
                a = int(input("Ingresa el valor de a: "))
                inverso = modInver(n, a)
                
                # Si cuenta con inverso modular, entonces...
                if inverso != -1:
                    print(f"{a}^-1 mod {n} = {inverso}")
                else:
                    print(f"{a} no se encuentra en el conjunto Z{n}*")
            
            print("_____________________________")

        case 3:       # Opción para generar claves aleatorias
            n = int(input("Ingresa el tamaño del alfabeto(n): "))
            randomKeys(n)
            
            if n < 1:
                print("¡Ingresa un número mayor que 2!")

            else:
                lista = Zn(n)

            # Imprime el conjunto Zn*
            print("Z" + str(n) + "* = {", end="")
            for i in lista:
                print(i, end="")
                if i != lista[-1]:
                    print(", ", end="")
            print("}")

            print("_____________________________")

        case 0:
            break
            
        case _:
            print("¡Ingresa una opción válida! :(")
            print("_____________________________")
        
    print("")
