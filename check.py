import xmltodict
import json
xml='''<grandfather>
    <father>
        <child name="Jose" bestFriend="Maria"/>
        <child name="Maria" bestFriend="Jose"/>
        <student favoriteBook="The Gods Themselves" age="21"/>
    </father>
</grandfather>'''
my_dict=xmltodict.parse(xml)
indentation=4
json_data=json.dumps(my_dict, indent=indentation) # add 'sort_keys=True)' if desired
print(json_data)
