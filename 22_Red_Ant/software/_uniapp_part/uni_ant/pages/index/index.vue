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
			
			
		</view>
	</view>
</template>

<script>
	export default {
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
						this.Temp = res.data.data.datastreams[2].datapoints[0].value;
						this.Humi = res.data.data.datastreams[3].datapoints[0].value;
						this.ligh = res.data.data.datastreams[4].datapoints[0].value;
						this.latitude = res.data.data.datastreams[0].datapoints[0].value;
						this.longitude = res.data.data.datastreams[1].datapoints[0].value;
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
	  height: 60rpx;
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
	
	
</style>


