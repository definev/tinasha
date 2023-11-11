import 'dart:convert';
import 'package:app/constant/constants.dart';
import 'package:http/http.dart' as http;

class StateListController {
  static Future<List<dynamic>> fetchData() async {
    final apiUrl = 'http://homeassistant.local:8123/api/states';

    final response = await http.get(
      Uri.parse(apiUrl),
      headers: {
        'Authorization': 'Bearer ${APIConstants.apiHass}',
        'Content-Type': 'application/json',
      },
    );

    if (response.statusCode == 200) {
      final List<dynamic> data = json.decode(response.body);
      return data;
    } else {
      throw Exception('Failed to load data');
    }
  }
  static Future<Map<String, dynamic>?> fetchEntityData(String entityID) async {
    final apiUrl = 'http://homeassistant.local:8123/api/states/$entityID';

    final response = await http.get(
      Uri.parse(apiUrl),
      headers: {
        'Authorization': 'Bearer ${APIConstants.apiHass}',
        'Content-Type': 'application/json',
      },
    );

    if (response.statusCode == 200) {
      return json.decode(response.body);
    } else {
      throw Exception('Failed to load data');
    }
  }
}