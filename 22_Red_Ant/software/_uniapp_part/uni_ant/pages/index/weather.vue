<template>
	<view class='out'>
		<image :src="bg" class="background" mode=""></image>
		<!-- #ifdef H5 -->
		
		<!-- #endif -->
		
		<!-- #ifdef MP-WEIXIN -->
		
		<!-- #endif -->
		
		<!-- 多端 - 兼容显示信息 -->
		<view class="main">
			<view class="search">
				<uni-icon type="spinner" size="14" color="#fff" class='search_icon'></uni-icon>
				<input class='search_input' type="text" value="" placeholder="输入城市名称 | 查询当地天气 (开发中)" placeholder-style='color: #fff' />
			</view>
			
			<view class="user_info flex_box_left padding">
				<image class="logo" src="/static/logo.png" />
<!-- 				<image :src="userInfo.logo" class='logo' mode=""></image> -->
				<view class="name">{{userInfo.name}}</view>
				<uni-icon type="more-filled" size="20" color="#fff" @click='handleClickMore'></uni-icon>
				<koraBg :futuretemp='futuretemp' :top='top' @bgHide='handleClickHideBg' @bgChange='bgChange'/>
			</view>
			<view class="tip_info flex_box_between padding">
				<view class="city flex_box_left">
					<uni-icon type="location" size="14" color="#fff"></uni-icon>
					<text> {{addressName}}</text>
					<uni-icon type="arrowdown" size="20" color="#fff" font-weight='600'></uni-icon>
				</view>
				<view class='fresh_time' @click='handleClickFreshTime'>{{freshData}} 更新</view>
			</view>
			
			<view class='temp_today'>
				<view class="temp flex_box_center">
					{{tempInfo.temp}} <text>℃</text>
				</view>
				<view class="flex_box_center">{{tempInfo.state}}</view>
				<view class="air flex_box_center">空气质量 {{tempInfo.air_level}}</view>
			</view>
			
			<!-- 未来天气 -->
			<tempList :futuretemp='futuretemp'/>
			
			<!-- 天气参数 -->
			<paramList :todayParam='todayParam'/>
			
			<!-- 温馨提示 -->
			<warmPrompt :todayInfo='todayInfo'/>
			
	<!-- 		<view class="author">开发者 · Kora</view> -->
			<!-- #ifdef H5 -->
<!-- 			<view class="author tip_h5">详细数据请查看小程序</view>
			<image class='mini_code' src="/static/img/qdtq.jpg" mode="widthFix"></image> -->
			<!-- #endif -->
			
<!-- 			<controlMap/> -->
		</view>
	</view>
</template>

<script>
	// 高德地图
	import amap from '../../common/amap-wx.js';
	// icon - UI
	import uniIcon from "@/components/uni-icon/uni-icon.vue"
	
	
	import koraBg from '@/components/kora-bg'
	import tempList from '@/components/index-temp'
	import paramList from '@/components/param-list'
	import warmPrompt from '@/components/warm-prompt'	
	import controlMap from '@/components/control-map'
	
	export default {
		data() {
			return {
				top: '-190',
				bg: '/static/img/bg0.jpg',
				userInfo: {
					logo: '/static/img/logo.png',
					name: 'Do what you want to do.'
				},
				tempInfo: {
					temp: 23,
					state: '晴',
					air_level: '优',
				},
				freshData: '',
				todayInfo: [],
				todayParam: [],
				futuretemp: Array(5).fill({date: '2023-06-09', tem2: '23℃', tem1: '24℃', wea: '晴', win: ['东风'], win_speed: '2~3级'}),
				amapPlugin: null,
				key: 'e7391e690c1e4ad35beedb24f4b1722b',
				addressName: '湛江',
				title: '网络请求',
				loading: false,
				res: '',
			}
		},
		components: {
			koraBg,
			uniIcon,
			tempList,
			paramList,
			warmPrompt,
			controlMap
		},
		async onLoad() {
			// #ifdef MP-WEIXIN
			
			// 获取更新时间
			this.freshTime()
			this.getWxTempInfo()
			
			// #endif
			
			// #ifdef H5
			
			uni.request({
				url: '/weather/api/?version=v1&city=' + '湛江', 
				success: (res) => {
					console.log(res);
				}
			})
			// #endif
		},
		methods: {
			handleClickGetUserInfo(e) {
				console.log(e)
			},
			handleClickGetLocal() {
				uni.getLocation({
					type: 'wgs84',
					success: function (res) {
						console.log('当前位置的经度：' + res.longitude);
						console.log('当前位置的纬度：' + res.latitude);
					}
				});
			},
			handleClickFreshTime() {
				this.freshTime()
				this.getWxTempInfo()
			},
			async getWxTempInfo() {
				let res = await this.$api.weather('湛江');
				console.log(res, '湛江天气')
				
				// 置空数据
				this.todayInfo = ''
				
				let todayInfos = res.data.data[0]
				// 基础数据
				this.tempInfo = { ...this.tempInfo, temp: todayInfos.tem[0] + todayInfos.tem[1], state: todayInfos.wea, air_level: todayInfos.air_level}
				
				// 七天预报
				this.futuretemp = res.data.data
				
				// 天气参数
				this.todayParam = [ {title: '温度（℃）', value: todayInfos.tem}, {title: '最低温度（℃）', value: todayInfos.tem2}, {title: '最高温度（℃）', value: todayInfos.tem1}, {title: '相对湿度（%）', value: todayInfos.humidity},  {title: '风向', value: todayInfos.win[0]}, {title: '风速（mk/h）', value: todayInfos.win_speed}, {title: '天气状况', value: todayInfos.wea}, {title: '云量', value: todayInfos.air}]
				
				// 温馨提示
				todayInfos.index.forEach((item, index, arr)=> {
					this.todayInfo = [...this.todayInfo, item]
				});
				this.todayInfo = [ { title: '旅游指数', level: '优', desc: '带着美好的心情，什么天气都是晴空万里' }, ...this.todayInfo, { title: '空气质量指数', level: todayInfos.air_level, desc: todayInfos.air_tips}, ].reverse()
			},
			freshTime() {
				var myDate = new Date()
				this.freshData = this.dateDeal(+myDate.getMonth() + 1) + '-' + this.dateDeal(myDate.getDate()) + ' ' + this.dateDeal(myDate.getHours())+ ':' + this.dateDeal(myDate.getMinutes())
			},
			dateDeal(x) {
				return ( x >= 10 ? x : ('0' + x ))
			},
			handleClickMore() {
				this.top = 0
			},
			handleClickHideBg() {
				this.top = '-190'
			},
			bgChange(e) {
				this.bg = '/static/img/bg' + e + '.jpg'
			}
		},
		onShareAppMessage(res) {

			return {
			  title: '湛江天气',
			  content: '湛江本周天气查询',
			  path: '/pages/index/index'
			}
		  }
		}
</script>

<style lang="scss">
	// @import "../../static/css/normalize.css";
	// @import "../../static/css/style.css";
	
	@import "../../static/css/default";

	.background{position: absolute; z-index: 1; @include postion-full-screen}
	.main{position: fixed; z-index: 2; overflow-y: scroll; @include postion-full-screen}
	.out{width: 100%; height: 100%}
	.search{@include flex-box(flex-start); position: relative; top: 0upx; left: 50%; transform: translateX(-50%); box-sizing: border-box; width: 90%; height: 80upx; border-bottom: 1upx solid #fff;
		.search_icon{position: relative; top: -3upx; left: 0}
		.search_input{margin-left: 20upx; line-height: 40upx; width: 90%; overflow: hidden; color: #fff; font-size: 28upx;
			&::placeholder{color: #fff}
		}
	}
	.user_info{margin: 26upx 0 10upx;
		>.logo{width: 80upx; height: 93upx; border-radius: 50%; overflow: hidden}
		>.name{margin: 0 15upx; font-size: 32upx; color: #fff}
	}
	.tip_info{color: #fff;
		.city{
			>text{margin: 0 6upx; font-size: 40upx; font-weight: 600; color: #fff}
		}
		.fresh_time{}
	}
	.temp_today{height: 500upx; color: #fff;
		& .temp{position: relative;  top: 0; left: 0; font-size: 190upx;
			text{position: absolute; top: 90upx; right: 28%; font-size: 37upx}
		}
		& .air{margin: 20upx auto 0; padding: 10upx 20upx; box-sizing: border-box; width: 200upx; line-height: 30upx; border-radius:20upx; background:rgba(240, 240, 240, .2)}
	}
	.mini_code{display: block;; margin: 20upx auto; width: 200upx;}
	.author{line-height: 80upx; text-align: center; color: #fff; font-size: 22upx}
	.padding{padding: 0 $padding}
	.flex_box_left{@include flex-box(flex-start)}
	.flex_box_center{@include flex-box(center)}
	.flex_box_between{@include flex-box(space-between)}
</style>
