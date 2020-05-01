import xmltodict
import json
xml='''<node attrib1="1" attrib2="2">
    <child child-attrib1="child1 attribute 1" child-attrib2="child1 attribute 2"/>
    <child child-attrib1="child2 attribute 1" child-attrib2="child2 attribute 2"/>
    <child grandchilds="little ones">
        <grandchild grandchild-attrib1="grandchild1 attribute 1"/>
        <grandchild grandchild-attrib1="grandchild2 attribute 1"/>
    </child>
    <child child-attrib1="child3 attribute 1" child-attrib2="child3 attribute 2"/>
</node>'''
my_dict=xmltodict.parse(xml)
indentation=2 # also jq default
json_data=json.dumps(my_dict, indent=indentation) # add 'sort_keys=True)' if desired
print(json_data)
