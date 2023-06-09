<template>
	<view class="wrapper">
		<view class="device-area">
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">温度</view>
					<image class="device-logo" src="/static/Temp.png"></image>
				</view>
				<view class="device-data">{{Temp}}℃</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">湿度</view>
					<image class="device-logo" src="/static/Humi.png"></image>
				</view>
				<view class="device-data">{{Humi}}%</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">光强</view>
					<image class="device-logo" src="/static//Ligh.png"></image>
				</view>
				<view class="device-data">{{ligh}}</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">补光灯</view>
					<image class="device-logo" src="/static/LED.png"></image>
				</view>
				<switch @change="onLedSwitch" color="#2b9939" :checked="Led"/>
			</view>
			
			<view class="device-cart" style="width: 100%;">
			  <view class="device-infox" style="display: flex; align-items: center;">
			    <image class="device-logox" src="/static/camera.png"></image>
			    <view class="device-namex">相机</view>
			  </view>
			  <button class="btnx" :class="{'btn-onx': camera}" @click="oncameraSwitch">{{ camera ? '' : '' }}</button>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">纬度</view>
					<image class="device-logo" src="/static/GPS.png"></image>
				</view>
				<view class="device-data">{{latitude}},N</view>
			</view>
			
			<view class="device-cart">
				<view class="device-info">
					<view class="device-name">经度</view>
					<image class="device-logo" src="/static/GPS.png"></image>
				</view>
				<view class="device-data">{{longitude}},E</view>
			</view>	
			
			
			<!-- 空格 -->
			<view class="device-cart2" style="width: 100%;">
			  <view class="device-infox" @click="navigateTo('/pages/index/weather')" style="display: flex; align-items: center;">
				<image class="weather-logox" src="/static/weather.png"></image>
			    <view class="device-namex">天气</view>
			  </view>
			  <!-- <button class="btny" :class="{'btn-onx': camera}" @click="oncameraSwitch">{{ camera ? '' : '' }}</button> -->
			</view>
			
			<!-- 切换页面按键 -->
			<view class="nav-bar">
			  <view class="nav-item" @click="navigateTo('/pages/index/index')">
			    <image class="nav-icon" src="/static/home.png"></image>
			    <text class="nav-text">首页</text>
			  </view>
			  <view class="nav-item" @click="navigateTo('/pages/index/login')">
			    <image class="nav-icon" src="/static/home2.png"></image>
			    <text class="nav-text">登录页</text>
			  </view>
			  <view class="nav-item" @click="navigateTo('/pages/index/CHAT')">
			    <image class="nav-icon" src="/static/openai.png"></image>
			    <text class="nav-text">CHAT</text>
			  </view>
			</view>
			
		</view>
	</view>

	    
	
</template>

<script>
	// import {getCurrentPosition, getWeather} from '@/utils/amap.js'
	// import $uni from '@dcloudio/uni-ui'
	
	export default {
		onLoad() {
		    this.$uni = $uni
		},
		data() {
			return {
				Temp: 0,
				Humi: 0,
				ligh: 0,
				latitude :0,
				longitude :0,
				Led: false,
				camera: false,
			}
		},
		onShow() {
			let that = this
			this.GetDatapoints()
			setInterval(function(){
				that.GetDatapoints()
				}, 3000);
		},
		
		onLoad() {
			
		},
		methods: {
			GetDatapoints: function(){
				uni.request({
				    url: 'http://api.heclouds.com/devices/1026301742/datapoints?', //仅为示例，并非真实接口地址。
				
				    header: {
				        'api-key': 'CQGBtAbN5qPrwI5PnVHNSvVmtNs=' //自定义请求头信息
				    },
					method: 'GET',
				    success: (res) => {
						this.Temp 	= res.data.data.datastreams[2].datapoints[0].value;
						this.Humi 	= res.data.data.datastreams[4].datapoints[0].value;
						this.ligh 	= res.data.data.datastreams[5].datapoints[0].value;
						this.latitude 	= res.data.data.datastreams[0].datapoints[0].value;
						this.longitude 	= res.data.data.datastreams[1].datapoints[0].value;
				    }
				});
			},
			onLedSwitch(event) {
				console.log(event.detail.value);
				let sw = event.detail.value;
				if (sw) {
					uni.request({
						url: 'http://api.heclouds.com/cmds?device_id=1026301742&qos=0&timeout=0&topic=LED _SW',
						header: {
							'api-key': 'CQGBtAbN5qPrwI5PnVHNSvVmtNs=' //Master-key
						},
						method: 'POST',
						data: JSON.stringify({"LEDSET":"1"}),
						success: (res) => {
							console.log("LEDSET on!");
						}
					});
				} else {
					uni.request({
						url: 'http://api.heclouds.com/cmds?device_id=1026301742&qos=0&timeout=0&topic=LED_SW',
						header: {
							'api-key': 'CQGBtAbN5qPrwI5PnVHNSvVmtNs=' //Master-key
						},
						method: 'POST',
						data: JSON.stringify({"LEDSET":"0"}),
						success: (res) => {
							console.log("LEDSET off!");
						}
					});
				}
			},
			oncameraSwitch(event) {
				console.log(event.detail.value);
				let sw = event.detail.value;
				if (!sw) {
					uni.request({
						url: 'http://api.heclouds.com/cmds?device_id=1026301742&qos=0&timeout=0&topic=CAMERA_SW',
						header: {
							'api-key': 'CQGBtAbN5qPrwI5PnVHNSvVmtNs=' //Master-key
						},
						method: 'POST',
						data: JSON.stringify({"CAMERA":"1"}),
						success: (res) => {
							console.log("CAMERA on!");
						}
					});
				} else {
					uni.request({
						url: 'http://api.heclouds.com/cmds?device_id=1026301742&qos=0&timeout=0&topic=CAMERA_SW',
						header: {
							'api-key': 'CQGBtAbN5qPrwI5PnVHNSvVmtNs=' //Master-key
						},
						method: 'POST',
						data: JSON.stringify({"CAMERA":"0"}),
						success: (res) => {
							console.log("CAMERA off!");
						}
					});
				}
			},
			navigateTo(url) {
			  uni.navigateTo({
			    url: url
			  })
			}
		}
	}
</script>

<style>
	.wrapper {
		padding: 30rpx;
	}

	.device-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.device-cart {
		width: 320rpx;
		height: 150rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		/* background-color: #8f8f94; */
		box-shadow: 0 0 15rpx #ccc;
	}
	
	.device-cart2 {
		width: 320rpx;
		height: 320rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		/* background-color: #8f8f94; */
		box-shadow: 0 0 15rpx #ccc;
	}
	
	.device-info {
		font-size: 20rpx;
		/* background-color: #8f8f94; */
	}
	
	.device-name{
		text-align: center;
		color: #6d6d6d;
	}
	.device-logo{
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}
	.weather-logox{
		width: 186rpx;
		height: 170rpx;
		margin-top: 10rpx;
	}
	.device-data{
		font-size: 50rpx;
		color: #6d6d6d;
	}
	.btn {
	  display: inline-block;
	  width: 36px;
	  height: 36px;
	  border-radius: 50%;
	  background-color: #fff;
	  border: 2px solid #ccc;
	}
	
	.device-cartx {
	  width: 100%;
	  display: flex;
	  justify-content: space-between;
	  align-items: center;
	  border-radius: 20rpx;
	  padding: 10rpx;
	  background-color: #ffffff;
	  box-shadow: 0 4rpx 12rpx rgba(0, 0, 0, 0.05);
	}
	
	.device-logox {
	  width: 66rpx;
	  height: 66rpx;
	  margin-left: 60rpx;
	  margin-right: 10rpx;
	}
	
	.device-infox {
	  display: flex;
	  align-items: center;
	}
	
	.device-namex {
	  font-size: 18rpx;
	  color: #333333;
	}
	
	.btnx {
	  width: 356rpx;
	  height: 66rpx;
	  border-radius: 10rpx;
	  border: 2rpx solid #cccccc;
	  background-color: #ffffff;
	  font-size: 14rpx;
	  color: #333333;
	  display: flex;
	  justify-content: center;
	  align-items: center;
	  outline: none;
	  cursor: pointer;
	}
	
	.btny {
	  width: 356rpx;
	  height: 200rpx;
	  border-radius: 10rpx;
	  border: 2rpx solid #cccccc;
	  background-color: #ffffff;
	  font-size: 14rpx;
	  color: #333333;
	  display: flex;
	  justify-content: center;
	  align-items: center;
	  outline: none;
	  cursor: pointer;
	}
	
	.btn-on {
	  border-color: #2b9939;
	  background-color: #2b9939;
	  color: #ffffff;
	}
	
	.nav-bar {
	    display: flex;
	    justify-content: space-between;
	    align-items: center;
	    position: fixed;
	    bottom: 0;
	    left: 0;
	    right: 0;
	    height: 50px;
	    background-color: #fff;
	    padding: 0 10px;
	    box-shadow: 0 -2px 4px rgba(0, 0, 0, 0.1);
/* 	  height: 140rpx;
	  width: 880rpx; */
	}
	
	.nav-item {
	  display: flex;
	  flex-direction: column;
	  justify-content: center;
	  align-items: center;
	  height: 100%;
	  width: 33.33%;
	}
	
	.nav-icon {
	  width: 20px;
	  height: 20px;
	}
	
	.nav-text {
	  font-size: 12px;
	  margin-top: 5px;
	  text-align: center;
	  color: #888;
	}
	
	.nav-item.active .nav-text {
	  color: #2b9939;
	}
	
	.nav-item:not(:last-child) {
	  border-right: 1px solid #eee;
	}
	
</style>

