import 'dart:convert';
import 'package:app/view/floating_action_button_view.dart';
import 'package:app/view/pick_file/single_file_picker.dart';
import 'package:app/view/speaking_setting_view/speaking_setting_view.dart';
import 'package:app/view/volume_screen.dart';
import 'package:flutter/material.dart';
import 'package:flutter_animate/flutter_animate.dart';
import 'package:http/http.dart' as http;
import 'package:app/controller/state_controller.dart';

class StateListScreen extends StatefulWidget {
  @override
  _StateListScreenState createState() => _StateListScreenState();
}

class _StateListScreenState extends State<StateListScreen>
    with SingleTickerProviderStateMixin {
  late AnimationController controller = AnimationController(vsync: this);
  List<dynamic> stateData = [];
  Map<String, dynamic>? entityData;

  @override
  void initState() {
    super.initState();
    fetchData();
    fetchTempData();
  }

  Future<void> fetchData() async {
    try {
      final data = await StateListController.fetchData();
      setState(() {
        stateData = data;
      });
    } catch (e) {
      // Xử lý lỗi ở đây nếu cần
      print('Failed to fetch data1: $e');
    }
  }

  Future<void> fetchTempData() async {
    try {
      final data =
          await StateListController.fetchEntityData('weather.forecast_home');
      setState(() {
        entityData = data;
      });
    } catch (e) {
      print('Failed to load data: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    Animate.restartOnHotReload = true;
    return Scaffold(
        bottomNavigationBar: BottomAppBar(
          child: SizedBox(
            height: 80,
            child: Row(
              children: [
                Expanded(
                  child: IconButton(
                    onPressed: () {},
                    icon: const Icon(
                      Icons.home_outlined,
                    ),
                    iconSize: 32,
                    color: Colors.blueAccent,
                  ),
                ),
                Expanded(
                  child: IconButton(
                    onPressed: () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                            builder: (context) => const VolumeScreen()),
                      );
                    },
                    icon: const Icon(
                      Icons.pie_chart_outline,
                    ),
                    iconSize: 32,
                    color: Colors.grey,
                  ),
                ),
                Expanded(
                    child: FloatingActionButton(
                  elevation: 0,
                  onPressed: () {
                    showDialog(
                      context: context,
                      builder: (context) => const FloatingActionButtonView(),
                    );
                    // controller.reverse();
                  },
                  child: Icon(Icons.add),
                )
                        .animate(
                          controller: controller,
                        )
                        .fadeIn(delay: 1.ms, duration: 50.ms)),
                Expanded(
                  child: IconButton(
                    onPressed: () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                            builder: (context) => const SpeakingSettingView()),
                      );
                    },
                    icon: const Icon(
                      Icons.switch_right_rounded,
                    ),
                    iconSize: 32,
                    color: Colors.grey,
                  ),
                ),
                Expanded(
                  child: IconButton(
                    onPressed: () {},
                    icon: const Icon(
                      Icons.settings_outlined,
                    ),
                    iconSize: 32,
                    color: Colors.grey,
                  ),
                ),
              ],
            ),
          ),
        ),
        // appBar: AppBar(
        //   title: Text('State List'),
        // ),
        body: Container(
          child: Column(
            children: [
              SafeArea(
                child: Padding(
                  padding: const EdgeInsets.only(left: 16, right: 16, top: 16),
                  child: Row(
                    children: [
                      Container(
                        height: 54,
                        width: 54,
                        decoration: BoxDecoration(
                          border: Border.all(),
                          shape: BoxShape.circle,
                        ),
                        child: const Icon(Icons.apps_outlined),
                      ),
                      Expanded(
                        child: Column(
                          children: const [
                            Text(
                              "Hello, Dreamwalker",
                              style: TextStyle(
                                fontWeight: FontWeight.bold,
                                fontSize: 18,
                              ),
                            ),
                            SizedBox(
                              height: 4,
                            ),
                            Text("Monday, 19 August 2022")
                          ],
                        ),
                      ),
                      const CircleAvatar(
                        radius: 27,
                      )
                    ],
                  ),
                ),
              ),
              Container(
                margin: const EdgeInsets.all(16),
                height: 160,
                decoration: BoxDecoration(
                  color: Colors.grey[100],
                  borderRadius: BorderRadius.circular(16),
                ),
                padding: const EdgeInsets.all(16),
                child: Column(
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            entityData != null &&
                                    entityData?['attributes'] != null
                                ? Text(
                                    '${entityData!['attributes']['temperature']} °C',
                                    style: TextStyle(
                                      fontSize: 28,
                                      fontWeight: FontWeight.bold,
                                    ),
                                  )
                                : Text(
                                    'N/A', // Hoặc một giá trị mặc định khác
                                    style: TextStyle(
                                      fontSize: 28,
                                      fontWeight: FontWeight.bold,
                                    ),
                                  ),
                            Text(
                              "It's pretty cloudy in outside.",
                            ),
                          ],
                        ),
                        IconButton(
                          icon: Icon(Icons.refresh),
                          onPressed:
                              fetchTempData, // Gọi _refresh khi nút được nhấn
                        ),
                      ],
                    ),
                    const SizedBox(
                      height: 16,
                    ),
                    Expanded(
                      child: Row(
                        children: [
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                entityData != null &&
                                        entityData?['attributes'] != null
                                    ? Text(
                                        '${entityData!['attributes']['wind_speed']} km/h',
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      )
                                    : Text(
                                        'N/A', // Hoặc một giá trị mặc định khác
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      ),
                                SizedBox(
                                  height: 8,
                                ),
                                Text(
                                  "Wind velocity",
                                  style: TextStyle(
                                    color: Colors.grey,
                                    fontWeight: FontWeight.bold,
                                  ),
                                ),
                              ],
                            ),
                          ),
                          const SizedBox(
                            width: 16,
                          ),
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                entityData != null &&
                                        entityData?['attributes'] != null
                                    ? Text(
                                        '${entityData!['attributes']['humidity']} %',
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      )
                                    : Text(
                                        'N/A', // Hoặc một giá trị mặc định khác
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      ),
                                SizedBox(
                                  height: 8,
                                ),
                                Text(
                                  "Home Humidity",
                                  style: TextStyle(
                                    color: Colors.grey,
                                    fontWeight: FontWeight.bold,
                                  ),
                                ),
                              ],
                            ),
                          ),
                          const SizedBox(
                            width: 16,
                          ),
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                entityData != null &&
                                        entityData?['attributes'] != null
                                    ? Text(
                                        '${entityData!['attributes']['pressure']} ${entityData!['attributes']['pressure_unit']}',
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      )
                                    : Text(
                                        'N/A', // Hoặc một giá trị mặc định khác
                                        style: TextStyle(
                                          fontSize: 16,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      ),
                                SizedBox(
                                  height: 8,
                                ),
                                Text(
                                  "Pressure",
                                  style: TextStyle(
                                    color: Colors.grey,
                                    fontWeight: FontWeight.bold,
                                  ),
                                ),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: const [
                    Text(
                      "Devices",
                      style: TextStyle(
                        fontSize: 24,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                  ],
                ),
              ),
              Expanded(
                child: RefreshIndicator(
                  onRefresh: () => fetchData(),
                  // fetchData() sẽ được gọi khi làm mới
                  child: ListView.builder(
                    itemCount: stateData.length,
                    itemBuilder: (context, index) {
                      final item = stateData[index];
                      return GestureDetector(
                        onTap: () {},
                        child: Container(
                          margin: EdgeInsets.all(10),
                          child: Card(
                            child: Padding(
                              padding: const EdgeInsets.all(10.0),
                              child: Column(
                                  crossAxisAlignment: CrossAxisAlignment.start,
                                  children: [
                                    Text(
                                      item['entity_id'],
                                      style: TextStyle(
                                          fontWeight: FontWeight.bold),
                                    ),
                                    Text('State: ${item['state']}'),
                                    Text(
                                        'Last Changed: ${item['last_changed']}'),
                                  ]),
                            ),
                          ),
                        ),
                      );
                    },
                  ),
                ),
              ),
            ],
          ),
        ));
  }
}
