import staticmaps
import cv2
import numpy as np
import os

cxtCarto = staticmaps.Context()
cxtCarto.set_tile_provider(staticmaps.tile_provider_CartoNoLabels)

cxtImagry = staticmaps.Context()
cxtImagry.set_tile_provider(staticmaps.tile_provider_ArcGISWorldImagery)

size = (500, 515)


def getCartoTile(center, zoom, size):
    cxtCarto.set_center(center)
    cxtCarto.set_zoom(zoom)
    return cxtCarto.render_pillow(size[0], size[1])


def getImagryTile(center, zoom, size):
    cxtImagry.set_center(center)
    cxtImagry.set_zoom(zoom)
    return cxtImagry.render_pillow(size[0], size[1])

nmas = 4
# toor = "D:/Programs/C++/Barcode/PrjBarlib/researching/"
def createImages(coordsPath):
    dirbase = os.path.dirname(__file__)
    dir = os.path.join(dibase, 'testimgs')
    if not os.path.exists(dir):
        os.makedirs(dir)
    k = 0
    with open(coors, 'r') as fileparams:
        for line in fileparams:
            if len(line) == 0 or line[0] == '#':
                continue

            prm = line.split('|')
            if len(prm) != 3:
                continue

            print(line[:-1])
            center = staticmaps.create_latlng(float(prm[0]), float(prm[1]))
            zoom = int(prm[2])

            img1 = getImagryTile(center, zoom, size)
            img1 = np.array(img1)[:-15,:]
            # cv2.imwrite(os.path.join(dir, str(k + 1) + '.png'), img1)
            cv2.imwrite(os.path.join(dir, str(k) + '_set.png'), img1)
            # # cv2.imwrite(os.path.join(dir, '1_cat.png'), img2)

            # imgMap = cv2.GaussianBlur(img1,(3,3),0)
            # cv2.imwrite(os.path.join(dir, str(k) + '_g3.png'), imgMap)

            k += 1


dirbase = os.path.dirname(__file__)
coors = os.path.join(dirbase, f'coords.txt')

createImages()