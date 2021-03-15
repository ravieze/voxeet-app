import 'react-native-gesture-handler';
import React, { useState, useEffect } from 'react';
import { View, Text } from 'react-native';
import { VoxeetSDK, ConferenceUser } from '@voxeet/react-native-voxeet-conferencekit';

const Dolby = (props) => {
  

  useEffect(() => {
    VoxeetSDK.initialize(consumerKey, consumerSecret)
      .then(() => voxeetSDK.connect(new ConferenceUser("76091449169221_QA_streamstyle", "76091449169221_QA_streamstyle", "")) //externalId and avatarUrl are optional
        .then(() => voxeetSDK.create({ alias: "76091449169221_QA_streamstyle" }))
        .then(({ conferenceId }) => voxeetSDK.join(conferenceId))
        .then(() => alert("joined"))
        .catch((e) => console.log('Voxeet join failed with error: ', e)));
  }, []);

  return (<>hi</>)

};

export default Dolby;
