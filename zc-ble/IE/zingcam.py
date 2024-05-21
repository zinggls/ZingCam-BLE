BND = {
    0 : "",
    ord("L") : "Low",
    ord("H") : "High"
}
FMT = { 
    0 : "",
    1 : "MJPEG",
    2 : "YUY2",
    3 : "UYVY"
}
IDX = {
    0 : "",
    1 : "1920x1080",
    2 : "1280x720",
    3 : "640x480",
    4 : "352x288",
    5 : "320x240",
    6 : "800x600"
}
TRT = {
    0 : "",
    ord("I") : "Isochronous",
    ord("B") : "Bulk"
}
ACK = {
    0 : "",
    ord("N") : "No ACK",
    ord("A") : "ACK"
}
PPC = {
    0 : "",
    ord("P") : "PPC",
    ord("D") : "DEV"
}
RUN = {
    0 : "",
    ord("Y") : "Running",
    ord("N") : "Not running"
}
ITF = {
    0 : "",
    ord("Y") : "O",
    ord("N") : "X"
}

def FPSCAL(HEX):
    FPS = 10000000 / HEX
    return int(FPS)