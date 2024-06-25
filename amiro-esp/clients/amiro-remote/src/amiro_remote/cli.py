import argparse
import time
import sys
import os
import json
from amiro_remote.can import CANReaderWriter
from amiro_remote.amiro import AMiRoDef
from amiro_remote.amiro_basyx import AmiroBasyx
import pandas

functions = ["light", "motor", "monitor", "benchmark_readerwriter",  # deactivated for indin23 "dashboard"
             "light_chaser", "print_aas", "obstacle_avoidance", "line_following", "calibrate_ring"]

parser = argparse.ArgumentParser(description='Communicate with an AMiRo remotely', formatter_class=argparse.ArgumentDefaultsHelpFormatter, add_help=False)
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument("--host", help="Connect to AMiRo via IP or hostname")
group.add_argument("--file", help="Load recorded AMiRo communication from file")
parser.add_argument("--amiro-id", help="Use AAS description")

parser.add_argument("--port", default=1234, type=int, help="AMiRo TCP Port")
parser.add_argument("--aas-server-url", default="http://pyke.techfak.uni-bielefeld.de:9000", help="basyx server url") # "http://129.70.134.234:8081/")

parser.add_argument("function", choices=functions, help="What to do")


def gen_client(args):
    if args.file:
        return AmiroRemoteClient(args.file)
    else:
        return AmiroRemoteClient((args.host, args.port))


def gen_readerwriter(args):
    amiro_def = AMiRoDef()
    if args.amiro_id:
        amiro_basyx = AmiroBasyx(args.aas_server_url)
        c_by_type = amiro_basyx.get_amiro_parsed_capabilities(args.amiro_id, False)
    else:
        amiro_basyx = AmiroBasyx(None)
        with open(f'{os.path.dirname(os.path.abspath(__file__))}/msg_types.json') as f:
            c_by_type = json.load(f)
        c_by_type[0] = c_by_type.pop("0")
        c_by_type[1] = c_by_type.pop("1")
    amiro_basyx.load_and_add_classes(c_by_type)
    amiro_def.load_message_defs(c_by_type)
    if args.file:
        return CANReaderWriter(args.file, amiro_def)
    else:
        return CANReaderWriter((args.host, args.port), amiro_def)


def light():
    colors = ["off", "red", "green", "blue", "yellow", "cyan", "magenta", "purple", "orange"]
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--by-color", choices=colors)
    group.add_argument("--by-index", type=int)
    args = parser.parse_args()

    from amiro_remote.msg_headers.LightRing_leddata import light_led_data_t
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_LIGHT'), f"SERVICEID_LIGHT not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        if args.by_index is not None:
            color_idx = int(args.by_index)
        if args.by_color:
            color_idx = colors.index(args.by_color.lower())

        msg = light_led_data_t()
        for i in range(len(msg.color)):
            msg.color[i] = color_idx
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_LIGHT'], msg)


def light_chaser():
    parser.add_argument("--color-start", type=int, default=9)
    parser.add_argument("--color-end", type=int, default=55)
    parser.add_argument("--sleep-time", type=float, default=0.05)

    args = parser.parse_args()

    from amiro_remote.msg_headers.LightRing_leddata import light_led_data_t
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_LIGHT') , f"SERVICEID_LIGHT not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        msg = light_led_data_t()
        for i in range(2):
            for color_idx in range(args.color_start, args.color_end):
                for i in range(len(msg.color)):
                    msg.color[i] = color_idx
                can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_LIGHT'], msg)
                time.sleep(args.sleep_time)
            for color_idx in range(args.color_end, args.color_start, -1):
                for i in range(len(msg.color)):
                    msg.color[i] = color_idx
                can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_LIGHT'], msg)
                time.sleep(args.sleep_time)
        for i in range(len(msg.color)):
            msg.color[i] = 0
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_LIGHT'], msg)


def motor():
    def check_float(arg):
        try:
            val = float(arg)
        except ValueError:
            raise argparse.ArgumentTypeError(f"{arg} not a float")
        return val

    parser.add_argument("translation", type=check_float)
    parser.add_argument("rotation", type=check_float)
    args = parser.parse_args()

    from amiro_remote.msg_headers.motiondata import struct_motion_ev_csi
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_DMC_TARGET'), f"SERVICEID_DMC_TARGET not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        t = struct_motion_ev_csi()
        t.translation.axes[0] = args.translation
        t.rotation.vector[2] = args.rotation
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_DMC_TARGET'], t)

def obstacle_avoidance():
    parser.add_argument("--strategy", type=int, default=0)
    parser.add_argument("--with-light", action="store_true")
    args = parser.parse_args()
    from amiro_remote.msg_headers.ObstacleAvoidanceData import oa_activation_service_t
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_OA_ACTIVATION'), f"SERVICEID_OA_ACTIVATION not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        t = oa_activation_service_t()
        t.light.enable_light = args.with_light
        t.strategy = args.strategy
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_OA_ACTIVATION'], t)

def calibrate_ring():
    args = parser.parse_args()
    from amiro_remote.msg_headers.ProximitySensordata import proximitysensor_calibration_t, PROXIMITYSENSOR_CALIBRATION_AUTO
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_PROXIMITYSENSOR_CALIBRATION'), f"SERVICEID_PROXIMITYSENSOR_CALIBRATION not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        t = proximitysensor_calibration_t()
        t.request.command = PROXIMITYSENSOR_CALIBRATION_AUTO
        t.request.data.measurements = 10
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_PROXIMITYSENSOR_CALIBRATION'], t)

def line_following():
    parser.add_argument("--state", type=int, default=0)
    parser.add_argument("--strategy", type=int, default=0)
    args = parser.parse_args()
    from amiro_remote.msg_headers.LineFollowingData import lf_activation_service_t
    with gen_readerwriter(args) as can_rw:
        assert hasattr(can_rw.amiro_def.Service, 'SERVICEID_LF_ACTIVATION'), f"SERVICEID_LF_ACTIVATION not support on your AMiRo ({list(can_rw.amiro_def.Service)})"
        t = lf_activation_service_t()
        t.state = args.state
        t.strategy = args.strategy
        can_rw._send_service(can_rw.amiro_def.Service['SERVICEID_LF_ACTIVATION'], t)

def payload_printer(payload):
    first = False
    for t in payload._fields_:
        t_a = getattr(payload, t[0])
        if hasattr(t_a, '_fields_'):
            payload_printer(t_a)
            print("")
        else:
            print(t_a, end=":" if first else " ")
            first = False


def monitor():
    parser.add_argument("--decode-payload", action="store_true")
    args = parser.parse_args()

    with gen_readerwriter(args) as can_rw:
        try:
            while True:
                data = can_rw.get()
                print(data)
                if args.decode_payload:
                    payload_printer(data.payload)
        except KeyboardInterrupt:
            return


def benchmark_readerwriter():
    import cProfile
    parser.add_argument("--num-items", default=2000)
    args = parser.parse_args()

    with gen_readerwriter(args) as can_rw:
        with cProfile.Profile() as pr:
            try:
                for i in range(args.num_items):
                    data = can_rw.get()
                    dummy = str(data)  # noqa: F841 dummy to make sure data is decoded
            except KeyboardInterrupt:
                return
            pr.print_stats(sort='cumulative')


def benchmark_amiroremoteclient():
    import cProfile
    parser.add_argument("--num-items", default=2000)
    args = parser.parse_args()

    with cProfile.Profile() as pr:
        with gen_client(args) as arc:
            try:
                for i in range(args.num_items):
                    data = arc.get()
                    dummy = str(data)  # noqa: F841 dummy to make sure data is decoded
            except KeyboardInterrupt:
                return
        pr.print_stats(sort='cumulative')


def dashboard():
    args = parser.parse_args()
    live_plotting(gen_client(args))


def print_aas():
    args = parser.parse_args()
    amiro_def = AMiRoDef()
    assert args.amiro_id, "You need to specify an AMiRo ID (--amiro-id)"
    ab = AmiroBasyx(args.aas_server_url)
    print(f"AMiRo {args.amiro_id}:")
    print("\nAsset Tree:")
    ab.pretty_print_asset_tree(ab.get_amiro_asset_tree(args.amiro_id))
    print("\nCapabilities:")
    c_by_type = ab.get_amiro_parsed_capabilities(args.amiro_id, False)
    ab.load_and_add_classes(c_by_type)
    ab.pretty_print_amiro_caps(c_by_type)


def save_barcode():
    args = parser.parse_args()
    db = []
    odom = [0, 0, 0]
    with gen_readerwriter(args) as can_rw:
        try:
            print("Starting")
            can_rw._send_service(Service.DMCTarget, [0.1, 0, 0, 0, 0, 0.0])
            while True:
                data = can_rw.get()
                if data.topic == Topic.Odometry:
                    odom = data.values
                elif data.topic == Topic.FloorProximity:
                    db.append(pandas.DataFrame({'odom': [odom[0]], 'floor': [data.values[3]]}))
        except KeyboardInterrupt:
            can_rw._send_service(Service.DMCTarget, [float("nan"), 0, 0, 0, 0, float("nan")])
            tmp = pandas.concat(db, ignore_index=True)
            tmp.to_csv('barcode.csv')
            t = tmp.plot()
            t.get_figure().savefig('barcode_index.pdf')
            t = tmp.groupby(by='odom').mean().plot()
            t.get_figure().savefig('barcode_pos.pdf')
            t = tmp.plot(kind='scatter', x='odom', y='floor')
            t.get_figure().savefig('barcode_scatter.pdf')
            print("After concat:")
            print(tmp)
            return


def main(arguments: list[str] = sys.argv[1:]) -> None:
    args, _ = parser.parse_known_args(arguments)
    parser.add_argument('--help', '-h', action="help")
    globals()[args.function]()


if __name__ == "__main__":
    main(sys.argv[1:])
