#pragma once

#ifndef _H_SESSION
#define _H_SESSION

#include "crypto.h"
#include "crypto_man.h"

typedef uint32_t data_size_type;
typedef uint16_t port_type;
typedef uint16_t user_id_type;

void insLen(std::string& data);

namespace msgr_proto
{
	typedef uint16_t key_size_type;

	typedef int32_t port_type_l;
	constexpr port_type_l port_null = -1;

	typedef uint64_t session_id_type;

#ifdef ASIO_STANDALONE
    typedef asio::error_code error_code_type;
#else
	typedef boost::system::error_code error_code_type;
#endif

	typedef std::shared_ptr<asio::ip::tcp::socket> socket_ptr;

	class server;

	//Exceptions that can be safely ignored
	class msgr_proto_error :public std::runtime_error
	{
	public:
		msgr_proto_error() :std::runtime_error("Error in msgr_proto") {}
		msgr_proto_error(const char* err) :std::runtime_error(err) {}
		msgr_proto_error(const std::string& err) :std::runtime_error(err) {}
	};

	class proto_kit :public crypto::session
	{
	public:
		proto_kit(crypto::server& _srv, asio::io_service& _iosrv, crypto::id_type _id, crypto::provider& _crypto_prov)
			:crypto::session(_srv, _iosrv, _id),
			provider(_crypto_prov), d0(_crypto_prov.GetPublicKey())
		{}

		virtual void do_enc(crypto::task&) override;
		virtual void do_dec(crypto::task&) override;

		friend class pre_session;
	private:
		inline rand_num_type get_rand_num_send() { if (rand_num_send == std::numeric_limits<rand_num_type>::max()) rand_num_send = 0; else rand_num_send++; return boost::endian::native_to_little(rand_num_send); }
		inline rand_num_type get_rand_num_recv() { if (rand_num_recv == std::numeric_limits<rand_num_type>::max()) rand_num_recv = 0; else rand_num_recv++; return boost::endian::native_to_little(rand_num_recv); }

		crypto::provider& provider;
		crypto::provider::sym_encryptor e;
		crypto::provider::sym_decryptor d;
		crypto::provider::asym_encryptor e1;
		crypto::provider::asym_decryptor d0;
		session_id_type session_id;
		rand_num_type rand_num_send, rand_num_recv;

		std::string empty_string;
	};

	class pre_session : public std::enable_shared_from_this<pre_session>
	{
	protected:
		class pre_session_watcher
		{
		public:
			pre_session_watcher(server& _srv, const std::shared_ptr<pre_session>& _ptr) :srv(_srv), ptr(_ptr) {}
			pre_session_watcher(const pre_session_watcher&) = delete;
			pre_session_watcher(pre_session_watcher&&) = delete;
			~pre_session_watcher();
		private:
			server& srv;
			std::shared_ptr<pre_session> ptr;
		};
		friend class pre_session_watcher;
	public:
		pre_session(server& _srv, crypto::provider& _crypto_prov, crypto::server& _crypto_srv, port_type_l _local_port, asio::io_service& main_io_srv, asio::io_service& misc_io_srv, const socket_ptr& _socket)
			:priv(_crypto_prov.dh_priv_block_size), pubA(_crypto_prov.dh_pub_block_size), pubB(_crypto_prov.dh_pub_block_size), key(sym_key_size),
			crypto_prov(_crypto_prov), proto_data(_crypto_srv.new_session<proto_kit>(_crypto_prov)),
			e(proto_data->e), d(proto_data->d), e1(proto_data->e1),
			session_id(proto_data->session_id), rand_num_send(proto_data->rand_num_send), rand_num_recv(proto_data->rand_num_recv),
			main_io_service(main_io_srv), misc_io_service(misc_io_srv), socket(_socket), srv(_srv)
		{
			local_port = _local_port;
		}

		void shutdown() { exiting = true; if (!successful) { socket->close(); proto_data->stop(); } }

		port_type_l get_port() const { return local_port; }
		const std::string& get_key() const { return key_string; }

		virtual void start() = 0;
	private:
		virtual void stage1() = 0;
		virtual void stage2() = 0;
		virtual void sid_packet_done() = 0;
	protected:
		void read_key_header();
		void read_key();

		void write_secret();
		void read_secret_header();
		void read_secret();

		void write_iv();
		void read_iv();

		void read_session_id(int check_level, bool ignore_error = false);
		void read_session_id_body(int check_level);
		void write_session_id();

		CryptoPP::SecByteBlock priv, pubA, pubB, key;
		key_size_type pubB_size;
		std::unique_ptr<char[]> pubB_buffer;

		byte iv_buffer[sym_key_size];

		key_size_type key_size;
		std::unique_ptr<char[]> key_buffer;
		std::string key_string;

		data_size_type sid_packet_size;
		std::unique_ptr<char[]> sid_packet_buffer;
		int stage = 0;

		crypto::provider& crypto_prov;
		std::shared_ptr<proto_kit> proto_data;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption &e;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption &d;
		CryptoPP::ECIES<CryptoPP::ECP>::Encryptor &e1;
		session_id_type &session_id;
		rand_num_type &rand_num_send, &rand_num_recv;
		rand_num_type rand_num;

		asio::io_service &main_io_service, &misc_io_service;
		socket_ptr socket;

		server &srv;
		port_type_l local_port;
		std::weak_ptr<pre_session_watcher> watcher;
		volatile bool exiting = false, successful = false;
	};

	class pre_session_s :public pre_session
	{
	public:
		pre_session_s(port_type_l local_port, const socket_ptr& _socket, server& _srv, crypto::provider& _crypto_prov, crypto::server& _crypto_srv, asio::io_service& main_io_srv, asio::io_service& misc_io_srv)
			:pre_session(_srv, _crypto_prov, _crypto_srv, local_port, main_io_srv, misc_io_srv, _socket)
		{}

		virtual void start();
	private:
		virtual void stage1();
		virtual void stage2();
		virtual void sid_packet_done();
	};

	class pre_session_c :public pre_session
	{
	public:
		pre_session_c(port_type_l local_port, const socket_ptr& _socket, server& _srv, crypto::provider& _crypto_prov, crypto::server& _crypto_srv, asio::io_service& main_io_srv, asio::io_service& misc_io_srv)
			:pre_session(_srv, _crypto_prov, _crypto_srv, local_port, main_io_srv, misc_io_srv, _socket)
		{}

		virtual void start();
	private:
		virtual void stage1();
		virtual void stage2();
		virtual void sid_packet_done();
	};

	class session_base :public std::enable_shared_from_this<session_base>
	{
	public:
		static constexpr int priority_sys = 30;
		static constexpr int priority_msg = 20;
		static constexpr int priority_plugin = 15;
		static constexpr int priority_file = 10;

		typedef std::function<void()> write_callback;

		session_base(server& _srv, port_type_l _local_port, const std::string& _key_string);
		session_base(const session_base&) = delete;
		~session_base();

		void join();

		virtual void start() = 0;
		virtual void shutdown() = 0;

		virtual void send(const std::string& data, int priority, write_callback&& callback) = 0;
		virtual void send(std::string&& data, int priority, write_callback&& callback) = 0;

		virtual std::string get_address() const = 0;
		user_id_type get_id() const { return uid; }
		port_type_l get_port() const { return local_port; }
		const std::string& get_key() const { return key_string; }
	protected:
		template <typename... _Ty> void on_data(_Ty&&... val);

		std::string key_string;
		user_id_type uid;

		server &srv;
		port_type_l local_port;
	};
	typedef std::shared_ptr<session_base> session_ptr;
	typedef std::unordered_map<user_id_type, session_ptr> session_list_type;

	class virtual_session :public session_base
	{
	public:
		typedef std::function<void(const std::string&)> on_data_callback;

		virtual_session(server& _srv, const std::string& _name)
			:session_base(_srv, port_null, ""), name(_name)
		{}

		virtual void start() {}
		virtual void shutdown() {}

		virtual void send(const std::string& data, int priority, write_callback&& callback);
		virtual void send(std::string&& data, int priority, write_callback&& callback);

		void push(const std::string& data);
		void push(std::string&& data);

		virtual std::string get_address() const { return name; }

		void set_callback(on_data_callback&& _callback) { on_recv_data = std::move(_callback); }
	private:
		std::string name;
		on_data_callback on_recv_data;
	};

	class session : public session_base
	{
	private:
		static constexpr size_t read_buffer_size = 0x4000, read_max_size = 0x200000;

		struct write_task {
			write_task() {}
			write_task(const std::string& _data, int _priority, write_callback&& _callback) :data(_data), callback(std::move(_callback)), priority(_priority) {}
			write_task(std::string&& _data, int _priority, write_callback&& _callback) :data(std::move(_data)), callback(std::move(_callback)), priority(_priority) {}
			std::string data;
			write_callback callback;
			int priority;
		};
		typedef std::list<write_task> write_que_tp;

		class read_end_watcher
		{
		public:
			read_end_watcher(server& _srv, session& _s) :srv(_srv), s(_s) {}
			read_end_watcher(const read_end_watcher&) = delete;
			read_end_watcher(read_end_watcher&&) = delete;
			~read_end_watcher();

		private:
			server &srv;
			session &s;
		};
		friend class read_end_watcher;

		class write_end_watcher
		{
		public:
			write_end_watcher(server& _srv, session& _s) :srv(_srv), s(_s) {}
			write_end_watcher(const write_end_watcher&) = delete;
			write_end_watcher(write_end_watcher&&) = delete;
			~write_end_watcher();

			void set_normal() { normal_quit = true; }

		private:
			server &srv;
			session &s;
			bool normal_quit = false;
		};
		friend class write_end_watcher;
	public:
		session(server& _srv, port_type_l _local_port, const std::string& _key_string, const std::shared_ptr<proto_kit>& _proto_data,
			asio::io_service& _main_iosrv, asio::io_service& _misc_iosrv, socket_ptr&& _socket)
			:session_base(_srv, _local_port, _key_string),
			crypto_kit(_proto_data),
			main_iosrv(_main_iosrv), misc_iosrv(_misc_iosrv), socket(std::move(_socket)),
			read_buffer(std::make_unique<char[]>(read_buffer_size))
		{}

		~session()
		{
			if (!exiting)
			{
				try
				{
					shutdown();
				}
				catch (...) {}
			}
		}

		virtual void start();
		virtual void shutdown();

		virtual void send(const std::string& data, int priority, write_callback&& callback);
		virtual void send(std::string&& data, int priority, write_callback&& callback);

		virtual std::string get_address() const { return socket->remote_endpoint().address().to_string(); }
	private:
		void read_header(const std::shared_ptr<read_end_watcher>& watcher);
		void read_data(size_t sizeLast, const std::shared_ptr<std::string>& buf, const std::shared_ptr<read_end_watcher>& watcher);
		void process_data(const std::shared_ptr<std::string>& buf, const std::shared_ptr<read_end_watcher>& watcher);

		void send(std::shared_ptr<write_task>&& task);
		void write(const std::shared_ptr<write_end_watcher>& watcher);
		void write_end();

		std::shared_ptr<proto_kit> crypto_kit;

		asio::io_service &main_iosrv, &misc_iosrv;
		socket_ptr socket;

		std::unique_ptr<char[]> read_buffer;
		write_que_tp write_que;

		volatile bool exiting = false;
	};

	class server
	{
	public:
		server(asio::io_service& _main_io_service,
			asio::io_service& _misc_io_service,
			asio::ip::tcp::endpoint _local_endpoint,
			crypto::provider& _crypto_prov,
			crypto::server& _crypto_srv)
			:main_iosrv(_main_io_service), misc_iosrv(_misc_io_service),
			acceptor(main_iosrv, _local_endpoint), resolver(main_iosrv),
			crypto_prov(_crypto_prov), crypto_srv(_crypto_srv), e0str(_crypto_prov.GetPublicKeyString())
		{
		}

		server(asio::io_service& _main_io_service,
			asio::io_service& _misc_io_service,
			crypto::provider& _crypto_prov,
			crypto::server& _crypto_srv)
			:main_iosrv(_main_io_service), misc_iosrv(_misc_io_service),
			acceptor(main_iosrv), resolver(main_iosrv),
			crypto_prov(_crypto_prov), crypto_srv(_crypto_srv), e0str(_crypto_prov.GetPublicKeyString())
		{
		}

		~server()
		{
			if (!closing)
				shutdown();
		}

		void start() { if (!started) { started = true; do_start(); } }
		void shutdown();

		bool send_data(user_id_type id, const std::string& data, int priority, session::write_callback&& callback);
		bool send_data(user_id_type id, std::string&& data, int priority, session::write_callback&& callback);

		bool send_data(user_id_type id, const std::string& data, int priority) { return send_data(id, data, priority, []() {}); }
		bool send_data(user_id_type id, const std::string& data, int priority, const std::string& message) { return send_data(id, data, priority, [message]() {std::cout << message << std::endl; }); }
		bool send_data(user_id_type id, std::string&& data, int priority) { return send_data(id, std::move(data), priority, []() {}); }
		bool send_data(user_id_type id, std::string&& data, int priority, const std::string& message) { return send_data(id, std::move(data), priority, [message]() {std::cout << message << std::endl; }); }

		void pre_session_over(const std::shared_ptr<pre_session>& _pre, bool successful = false);
		void join(const session_ptr& _user, user_id_type& uid);
		void leave(user_id_type id);

		void connect(const std::string& addr, port_type remote_port);
		void connect(unsigned long addr, port_type remote_port);
		void disconnect(user_id_type id);

		session_base& get_session(user_id_type id) const { return *sessions.at(id); }
		const std::string& get_public_key() const { return e0str; }

		virtual bool new_key(const std::string&) { return true; }
		virtual void delete_key(const std::string&) {}

		void on_exception(const std::string& ex) noexcept { misc_iosrv.post([this, ex]() { on_error(ex.c_str()); }); }
		void on_exception(std::string&& ex) noexcept { misc_iosrv.post([this, ex]() { on_error(ex.c_str()); }); }
		void on_exception(std::exception& ex) noexcept { misc_iosrv.post([this, ex]() { on_error(ex.what()); }); }
		void on_exception(const char* ex) noexcept { misc_iosrv.post([this, ex]() { on_error(ex); }); }

		friend class session_base;
	protected:
		virtual void on_data(user_id_type id, const std::string& data) = 0;

		virtual void on_join(user_id_type id, const std::string& key) = 0;
		virtual void on_leave(user_id_type id) = 0;

		virtual bool new_rand_port(port_type& port) = 0;
		virtual void free_rand_port(port_type port) = 0;

		virtual void on_error(const char* err) { std::cerr << err << std::endl; }
	private:
		void do_start();

		void connect(const asio::ip::tcp::endpoint& remote_endpoint);
		void connect(const asio::ip::tcp::resolver::query& query);

		void on_recv_data(user_id_type id, const std::shared_ptr<std::string>& data);

		asio::io_service &main_iosrv, &misc_iosrv;
		asio::ip::tcp::acceptor acceptor;
		asio::ip::tcp::resolver resolver;

		crypto::provider& crypto_prov;
		crypto::server& crypto_srv;
		std::string e0str;

		std::unordered_set<std::shared_ptr<pre_session>> pre_sessions;
		session_list_type sessions;
		user_id_type nextID = 0;
		volatile user_id_type session_active_count = 0;

		std::mutex session_mutex, pre_session_mutex;
		volatile bool started = false, closing = false;
	};

	inline void session_base::join()
	{
		srv.join(shared_from_this(), uid);
	}

	template <typename... _Ty> inline void session_base::on_data(_Ty&&... val)
	{
		srv.on_recv_data(std::forward<_Ty>(val)...);
	}
}

#endif
