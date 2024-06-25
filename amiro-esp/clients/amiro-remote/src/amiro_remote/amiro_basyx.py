import re
import argparse
import sys
import importlib
from ml4proflow_mods.basyx.connectors import BasyxAASShellConnector
from ml4proflow_mods.basyx.submodels.sm_decoder import decode_submodel
from ml4proflow_mods.basyx.submodels.capabilities import parse_capabilities


class AmiroBasyx():
    def __init__(self, aas_shell_url: str) -> None:
        self.aas_server = BasyxAASShellConnector(aas_shell_url)

    def get_amiro_asset_tree(self, amiro_id: str):
        amiro_aas_id = f'http://amiro{amiro_id}/basyx/robot/aas'
        sm = self.aas_server.get_submodel(amiro_aas_id, 'HierarchicalStructures')
        asset_tree = decode_submodel(sm)
        return asset_tree

    def pretty_print_asset_tree(self, asset_tree, depth: int = 0):
        prefix = '|' if depth > 0 else ''
        for i in range(depth):
            prefix += '-'
        for k, v in asset_tree.items():
            print(f'{prefix}{k}')
            self.pretty_print_asset_tree(v, depth+1)

    def get_amiro_configuration_asset_id(self, amiro_id: str):
        asset_tree = self.get_amiro_asset_tree(amiro_id)
        # search for any configuration asset (it will be at depth 1 of the robot! (starts with: )
        for robot_assets in asset_tree.values():  # this is level 0 of the tree -> it has only one element -> robot
            for asset_id in robot_assets:  # this is level 1 if the robot has a configuration it will be here
                if re.match('https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-Apps/-/tree/\\w+/configurations/\\w+/asset',
                            asset_id):
                    return asset_id
            else:
                return None

    def get_amiro_capabilities(self, amiro_id: str):
        configuration_asset_id = self.get_amiro_configuration_asset_id(amiro_id)
        assert configuration_asset_id is not None, "Your robot asset doesn't contain a configuration asset"
        # Assume: Asset -> AAS only have same ID prefix!
        caps = self.aas_server.get_submodel(configuration_asset_id.replace('/asset', ''), 'capabilities')
        parsed_caps = parse_capabilities(caps)
        return parsed_caps

    def get_amiro_parsed_capabilities(self, amiro_id: str, skip_non_can_bus: bool = True):
        parsed_caps = self.get_amiro_capabilities(amiro_id)
        cap_by_type = {}
        for k, v in parsed_caps.items():
            if skip_non_can_bus:
                if 'support_can' not in v or not v['support_can']:
                    continue
            if int(v['type']) not in cap_by_type:
                cap_by_type[int(v['type'])] = []
            cap_by_type[int(v['type'])].append(v)
        # with open('test.json','w') as f:
        #    json.dump(cap_by_type, f)
        return cap_by_type

    def load_and_add_classes(self, cap_by_type):
        for v in cap_by_type.values():
            for i in v:
                if i['payload'] != "":
                    t = i['payload'].split(":")
                    t[0] = t[0].replace('.h', '')
                    tmp = importlib.import_module(f"amiro_remote.msg_headers.{t[0]}")
                    c = getattr(tmp, t[1])
                    i['class'] = c

    def pretty_print_amiro_caps(self, cap_by_type):
        for _type, items in cap_by_type.items():
            print(f"Group {_type}:")
            for i in items:
                c = str(i.get('class', '-'))
                print(f"    {i['id']:3} {i['name']:40s} {i['payload']:55} {i['description']:55} {c:20}")


def main(arguments: list[str] = sys.argv[1:]) -> None:
    parser = argparse.ArgumentParser(description='Communicate with an AMiRo remotely (basyx version)')
    parser.add_argument('--aas-url', default='http://129.70.134.234:8081/')
    # parser.add_argument('--registry-url', default='http://129.70.134.234:8082/')
    parser.add_argument('--auth', default=None, help='not implemented yet')
    parser.add_argument('--amiro-id', default=75)
    parser.add_argument('--skip-non-can', action="store_true")
    parser.add_argument('--verbose', default=75, action="store_true")
    args = parser.parse_args()
    # registry = BasyxRegistryConnector(args.registry_url)
    # reg = test.get_registry()
    # for x in SimpleJSONAASParser.get_aas_identifications_from_registry(reg):
    #    if x['id'].startswith('https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-Apps/-/tree/main/configurations/'):
    # this is an AAS for SW configuration
    #        print(f"AAS: {x}")
    ab = AmiroBasyx(args.aas_url)
    if args.verbose:
        print(f"AMiRo {args.amiro_id}:")
        print("\nAsset Tree:")
        ab.pretty_print_asset_tree(ab.get_amiro_asset_tree(args.amiro_id))
        print("\nCapabilities:")
        c_by_type = ab.get_amiro_parsed_capabilities(args.amiro_id, args.skip_non_can)
        ab.load_and_add_classes(c_by_type)
        ab.pretty_print_amiro_caps(c_by_type)


if __name__ == "__main__":
    main(sys.argv[1:])
