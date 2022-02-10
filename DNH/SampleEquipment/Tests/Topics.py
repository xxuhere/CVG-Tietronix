import sys
import json
import time

execfile("../RegisterUtils.py")

WSCONNECTION = f"ws://{GetDNHHostname()}:{GetDNHWSPort()}/realtime"

reg = CreateEqRegistration("Topics Test", "", "spectator")

print("Connecting first WS connection")
success1, ws1, guid1 = ConnectWS(WSCONNECTION, reg)
assert(success1)


def GetNextNonPing(ws):
    while True:
        recv = ws1.recv()
        if not recv:
            return None
        x = json.loads(recv)
        if x["apity"] == "ping":
            continue
        else:
            return x


SLEEP_BUFFER = 2.0


def SleepThenGetNonPing(ws):
    time.sleep(SLEEP_BUFFER)
    return GetNextNonPing(ws)


def PrintBanner(x):
    print("\n\t" + x)
    print("================================")


def SendAsJSON_Sleep_Banner_Print(ws, pyToJSON, banner):
    ws.send(json.dumps(pyToJSON))
    resp = SleepThenGetNonPing(ws)
    PrintBanner(banner)
    print(resp)
    return resp


# %% Subscribe to channels test1 and test2

sub = {}
sub["apity"] = "subscribe"
sub["mode"] = "add"
sub["topics"] = ["test1", "test2"]

resp = SendAsJSON_Sleep_Banner_Print(ws1, sub, "Subscribe Response")
assert(resp["apity"] == "subscribe")
assert(resp["mode"] == "add")
assert(resp["status"] == "success")

# %% Check the subscribe report feature

print("\n\nChecking subscriptions")
check = {}
check["apity"] = "subscribe"
check["mode"] = "report"
resp = SendAsJSON_Sleep_Banner_Print(ws1, check, "Subscribe Report Response")

assert(resp["apity"] == "subscribe")
assert(resp["mode"] == "report")
assert(resp["status"] == "success")
assert(resp["topics"] != None)
assert(len(resp["topics"]) == 2)
assert("test1" in resp["topics"])
assert("test2" in resp["topics"])

# %% Log in another connection

print("Connecting a second WS connection")
success2, ws2, guid2 = ConnectWS(WSCONNECTION, reg)

broad = {}
broad["apity"] = "publish"
broad["topic"] = "test1"
broad["data"] = {"val": "The thing!"}
resp = SendAsJSON_Sleep_Banner_Print(ws1, broad, "Broadcast Message")

# %%

r = ws1.recv()
print(r)

r = ws1.recv()
print(r)

# %%

print("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
print("ALL TESTS PASSED!")
ws1.close()
ws2.close()
