import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import Dolby  from "./Consumer-Dolby"

export default class App {

 render(){
  return (
    <View style={styles.container}>
    	<Dolby />
    </View>
  );
 }
}
