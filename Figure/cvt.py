import cv2

img = cv2.imread('BIT.jpg')
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)