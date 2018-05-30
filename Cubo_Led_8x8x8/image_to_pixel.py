from PIL import Image


# Promedio
def prgb(rgb):
    return int(round(sum(rgb) / 3, 0))


def scale(lista):
    return min(lista), max(lista)


def toggle(pixel: tuple, rango: tuple):
    if len(pixel) == 4:
        pixel = pixel[:-1]
    if prgb(pixel) != 0:
        return str(1)
    else:
        return str(0)


def image_to_byte(name: str) -> list:
    image = list()

    im = Image.open(name)
    pix = im.load()

    matrix = list()
    for i in range(0, 7):
        for j in range(0, 7):
            matrix.append(prgb(pix[j, i]))

    for i in range(8):
        image.append(list())
        for j in range(8):
            image[i].append(toggle(pix[j, i], scale(matrix)))
        image[i] = (int("".join(image[i]), 2))
    return image


if __name__ == "__main__":
    print(image_to_byte("crepper.png"))
