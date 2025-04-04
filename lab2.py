def map():

    return

n = 0
alfabeto = []
while(True):
    print("")
    print("[1] Asociar alfabeto con Zn.")
    print("[2] Vigenènere encipher.")
    print("[3] Vigenènere decipher.")
    print("[0] Salir")
    opc = int(input("Selecciona una opción: "))
    print("")

    match(opc):
        case 1:
            n = int(input("Ingresa el tamaño del alfabeto"))
            for i in range(n):
                c = input(f"Ingresa el elemento {i} del alfabeto: ")
                alfabeto.append(c)
            
            
        case 2:
            break

        case 3:
            break

        case 0:
            break
            
        case _:
            print("¡Ingresa una opción válida! :(")
            print("_____________________________")
        
    print("")