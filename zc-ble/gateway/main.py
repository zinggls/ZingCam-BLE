import multiprocessing
import ZG
import ZGG
import VLC

if __name__ == "__main__":
    manager = multiprocessing.Manager()
    host_status = manager.dict()
    device_status = manager.dict()
    host_status_name = manager.list(range(12 + 14))
    device_status_name = manager.list(range(12 + 12))
    camera = multiprocessing.Value('i', 0)
    player = multiprocessing.Value('i', 0)

    ZingGateway = multiprocessing.Process(target = ZG.ZingGateway, args = ("COM6", host_status, device_status, host_status_name, device_status_name, camera, player))
    ZingGateway.start()

    ZingGatewayGUI = multiprocessing.Process(target = ZGG.ZingGatewayGUI, args = (host_status, device_status, host_status_name, device_status_name, 14, 12))
    ZingGatewayGUI.start()

    VLCPlayer = multiprocessing.Process(target = VLC.VLCPlayer, args = (camera, host_status, device_status, player))
    VLCPlayer.start()

    ZingGatewayGUI.join()
    ZingGateway.join()