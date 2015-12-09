from PIL import Image
import stepic
import StringIO

def Steganify(fakeImagePath,realImagePath):#,pathToSaveTo):

    im = Image.open (fakeImagePath)
    im3 = Image.open(realImagePath)
    output = StringIO.StringIO()
    im3.save(output, format = "PNG")
    contents = output.getvalue()
    im2 = stepic.encode(im, contents)
    output.close()
    output = StringIO.StringIO()
    im2.save(output, format = "PNG")
    #im2.save(pathToSaveTo)
    encodedImage = output.getvalue()
    output.close()
    return encodedImage


def DeSteganify(imagePath):#,pathToSaveTo):
    im = Image.open(imagePath)
    s = stepic.decode(im)
    output = StringIO.StringIO(s)
    #image = Image.open(output)
    #image.save(pathToSaveTo+'decoded.png')
    decodedImage = output.getvalue()
    output.close()
    return decodedImage
